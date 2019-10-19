import { loadProgram, Buffer } from './render.js'

function loadToxicCalcPipe(gl) {
    if(loadToxicCalcPipe.pipe === undefined) {
        loadToxicCalcPipe.pipe = {};
        const vsCode= `
        attribute vec2 aPos;
        attribute vec2 aUv;

        varying lowp vec2 uv;

        void main() {
            gl_Position = vec4(aPos, 0, 1);
            uv = aUv;
        }
        `;

        const fsCode = `
        precision highp float;

        uniform sampler2D uSampler;
        uniform float dT;

        varying lowp vec2 uv;

        void main() {
            vec4 color = texture2D(uSampler, uv);
            color.a *= pow(0.5, dT);
            gl_FragColor = color;
        }
        `;

        loadToxicCalcPipe.pipe.prog = loadProgram(gl, vsCode, fsCode);
        const prog = loadToxicCalcPipe.pipe.prog;
        loadToxicCalcPipe.pipe.attrLoc = {
            pos: gl.getAttribLocation(prog, 'aPos'),
            uv: gl.getAttribLocation(prog, 'aUv'),
        };
        loadToxicCalcPipe.pipe.uniLoc = {
            sampler: gl.getUniformLocation(prog, 'uSampler'),
            dT: gl.getUniformLocation(prog, 'dT'),
        };
    }
    return loadToxicCalcPipe.pipe;
}

function loadToxicRenderPipe(gl) {
    if(loadToxicRenderPipe.pipe === undefined) {
        loadToxicRenderPipe.pipe = {};
        const vsCode = `
        attribute vec2 aPos;
        attribute vec2 aUv;

        uniform mat4 uProj;

        varying lowp vec2 uv;

        void main() {
            gl_Position = uProj * vec4(aPos, 0, 1);
            uv = aUv;
        }
        `;

        const fsCode = `
        precision lowp float;

        uniform sampler2D uSampler;
        uniform vec3 uColor;

        varying lowp vec2 uv;

        void main() {
            gl_FragColor = vec4(uColor, texture2D(uSampler, uv).a);
        }
        `;

        loadToxicRenderPipe.pipe.prog = loadProgram(gl, vsCode, fsCode);
        const prog = loadToxicRenderPipe.pipe.prog;
        loadToxicRenderPipe.pipe.attrLoc = {
            pos: gl.getAttribLocation(prog, 'aPos'),
            uv: gl.getAttribLocation(prog, 'aUv'),
        };
        loadToxicRenderPipe.pipe.uniLoc = {
            sampler: gl.getUniformLocation(prog, 'uSampler'),
            color: gl.getUniformLocation(prog, 'uColor'),
            proj: gl.getUniformLocation(prog, 'uProj'),
        };
    }
    return loadToxicRenderPipe.pipe;
}

class Toxic {
    constructor(gl, w, h, start, resolution, color = [255, 0, 0]) {
        this.w = w * resolution;
        this.h = h * resolution;
        this.color = color.map(function(x) { return x / 255.0; });
        this.gl = gl;
        this.tSrc = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, this.tSrc);
        gl.texImage2D(
            gl.TEXTURE_2D,
            0,
            gl.RGBA, this.w, this.h,
            0,
            gl.RGBA, gl.UNSIGNED_BYTE, start.map(function(x,id){ return id%4===3 ? x : 0; }));
        const setTexPara = function() {
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        }
        setTexPara();

        this.tDst = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, this.tDst);
        gl.texImage2D(
            gl.TEXTURE_2D,
            0,
            gl.RGBA, this.w, this.h,
            0,
            gl.RGBA, gl.UNSIGNED_BYTE, null);
        setTexPara();
        
        this.fbSrc = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbSrc);
        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.tSrc, 0);

        this.fbDst = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbDst);
        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.tDst, 0);

        gl.bindFramebuffer(gl.FRAMEBUFFER, null);

        this.computePipe = loadToxicCalcPipe(gl);
        this.renderPipe = loadToxicRenderPipe(gl);

        this.renderBuffer = {
            pos: new Buffer(gl, [0,h , w,h, 0,0, w,0], Float32Array, gl.ARRAY_BUFFER, gl.STATIC_DRAW),
            uv: new Buffer(gl, [0,1, 1,1, 0,0, 1,0], Float32Array, gl.ARRAY_BUFFER, gl.STATIC_DRAW),
        };

        this.computeBuffer = {
            pos: new Buffer(gl, [-1,1, 1,1, -1,-1, 1,-1], Float32Array, gl.ARRAY_BUFFER, gl.STATIC_DRAW),
            uv: this.renderBuffer.uv,
        };
    }

    draw(game) {
        this.gl.useProgram(this.renderPipe.prog);
        this.gl.enableVertexAttribArray(this.renderPipe.attrLoc.pos);
        this.gl.enableVertexAttribArray(this.renderPipe.attrLoc.uv);

        this.gl.activeTexture(this.gl.TEXTURE0);
        this.gl.uniform1i(this.renderPipe.uniLoc.sampler, 0);
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.tSrc);

        this.gl.uniform3fv(this.renderPipe.uniLoc.color, this.color);
        this.gl.uniformMatrix4fv(this.renderPipe.uniLoc.proj, false, game.camera.proj);

        this.renderBuffer.pos.bindAttribPointer(this.renderPipe.attrLoc.pos, 2, false, 0);
        this.renderBuffer.uv.bindAttribPointer(this.renderPipe.attrLoc.uv, 2, false, 0);
        
        this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

        this.gl.disableVertexAttribArray(this.renderPipe.attrLoc.pos);
        this.gl.disableVertexAttribArray(this.renderPipe.attrLoc.uv);
    }

    update(dT) {
        const viewport = this.gl.getParameter(this.gl.VIEWPORT);
        this.gl.viewport(0,0,this.w ,this.h);

        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbDst);

        this.gl.useProgram(this.computePipe.prog);
        this.gl.enableVertexAttribArray(this.computePipe.attrLoc.pos);
        this.gl.enableVertexAttribArray(this.computePipe.attrLoc.uv);

        this.gl.activeTexture(this.gl.TEXTURE0);
        this.gl.uniform1i(this.computePipe.uniLoc.sampler, 0);
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.tSrc);

        this.gl.uniform1f(this.computePipe.uniLoc.dT, dT);

        this.computeBuffer.pos.bindAttribPointer(this.computePipe.attrLoc.pos, 2, false, 0);
        this.computeBuffer.uv.bindAttribPointer(this.computePipe.attrLoc.uv, 2, false, 0);

        this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

        this.gl.disableVertexAttribArray(this.computePipe.attrLoc.uv);
        this.gl.disableVertexAttribArray(this.computePipe.attrLoc.pos);

        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
        this.gl.viewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        this.tSrc = [this.tDst, this.tDst = this.tSrc][0];
        this.fbSrc = [this.fbDst, this.fbDst = this.fbSrc][0];
    }
};

export function createToxic(gl, w, h, data, color) {
    return new Toxic(gl, w*2, h*2, data, 0.5, color);
}