import { loadProgram, Buffer } from './render.js'

function loadToxicCalcPipe(gl, type) {
    console.log(type);
    if(loadToxicCalcPipe[type].pipe === undefined) {
        loadToxicCalcPipe[type].pipe = {};
        const vsCode = `#version 300 es
        in vec2 aPos;
        in vec2 aUv;

        out highp vec2 uv;

        void main() {
            gl_Position = vec4(aPos, 0, 1);
            uv = aUv;
        }
        `;

        loadToxicCalcPipe[type].pipe.prog = loadProgram(gl, vsCode, loadToxicCalcPipe[type].fsCode);
        const prog = loadToxicCalcPipe[type].pipe.prog;
        loadToxicCalcPipe[type].pipe.attrLoc = {
            pos: gl.getAttribLocation(prog, 'aPos'),
            uv: gl.getAttribLocation(prog, 'aUv'),
        };

        loadToxicCalcPipe[type].pipe.uniLoc = loadToxicCalcPipe[type].bindUni(gl, prog);
    }
    return loadToxicCalcPipe[type].pipe;
}

loadToxicCalcPipe.advect = {
    fsCode: `#version 300 es
        precision highp float;

        uniform sampler2D uSampler;
        uniform float dT;

        in highp vec2 uv;

        layout(location = 0) out vec4 color;

        void main() {
            vec4 px = texture(uSampler, uv);
            color = texture(uSampler, uv - dT * px.xy); 
        }
        `,
    bindUni: function(gl, prog) {
        return {
            sampler: [gl.getUniformLocation(prog, 'uSampler')],
            dT: gl.getUniformLocation(prog, 'dT'),
        };
    }
};

loadToxicCalcPipe.diffuse = {
    fsCode: `#version 300 es
        precision mediump float;

        uniform sampler2D uSampler;
        uniform vec2 uSizePx;

        uniform float alpha;
        uniform float beta;
        uniform int nLayer;

        in highp vec2 uv;

        layout(location = 0) out vec4 color;
        void main() {
            vec4 l = texture(uSampler, uv + vec2(uSizePx.x, 0));
            vec4 r = texture(uSampler, uv - vec2(uSizePx.x, 0));
            vec4 d = texture(uSampler, uv + vec2(0, uSizePx.y));
            vec4 u = texture(uSampler, uv - vec2(0, uSizePx.y));

            color = texture(uSampler, uv);
            color[2] = (l[2] + r[2] + d[2] + u[2] + alpha * color[2]) * beta;
            color[3] = (l[3] + r[3] + d[3] + u[3] + alpha * color[3]) * beta;
        }
        `,
    bindUni: function(gl, prog) {
        return {
            sampler: [gl.getUniformLocation(prog, 'uSampler')],
            alpha: gl.getUniformLocation(prog, 'alpha'),
            beta: gl.getUniformLocation(prog, 'beta'),
            layer: gl.getUniformLocation(prog, 'nLayer'),
            size: gl.getUniformLocation(prog, 'uSizePx'),
        };
    }
};

loadToxicCalcPipe.projection = {
    fsCode: `#version 300 es
        precision mediump float;

        uniform sampler2D, uSampler;
        uniform vec2 uSizePx;

        uniform float alpha;

        in highp vec2 uv;

        layout(location = 0) out vec4 color;
        void main() {
            vec4 l = texture(uSampler, uv - vec2(uSizePx.x, 0));
            vec4 r = texture(uSampler, uv + vec2(uSizePx.x, 0));
            vec4 d = texture(uSampler, uv + vec2(0, uSizePx.y));
            vec4 u = texture(uSampler, uv - vec2(0, uSizePx.y));

            float div = (r.x - l.x) + (u.y - d.y);
            
            color = texture(uSampler, uv);
            color.w = (l.w + r.w+ d.w + u.w - 0.02 * div) * 0.25;
        }
    `,
    bindUni: function(gl, prog) {
        return {
            sampler: [gl.getUniformLocation(prog, 'uSampler')],
            alpha: gl.getUniformLocation(prog, 'alpha'),
            size: gl.getUniformLocation(prog, 'uSizePx'),
        };
    }
};

loadToxicCalcPipe.applyForce = {
    fsCode: `#version 300 es
        precision mediump float;

        uniform sampler2D uSampler;
        uniform vec2 uSizePx;
        uniform float dT;
        

        in highp vec2 uv;

        layout(location = 0) out vec4 color;
        void main() {
            vec4 l = texture(uSampler, uv - vec2(uSizePx.x, 0));
            vec4 r = texture(uSampler, uv + vec2(uSizePx.x, 0));
            vec4 d = texture(uSampler, uv + vec2(0, uSizePx.y));
            vec4 u = texture(uSampler, uv - vec2(0, uSizePx.y));

            color = texture(uSampler, uv);
            color.xy -= vec2(r.w - l.w, d.w - u.w);
            color.y += dT * color.b;
        }
    `,
    bindUni: function(gl, prog) {
        return {
            sampler: [gl.getUniformLocation(prog, 'uSampler')],
            size: gl.getUniformLocation(prog, 'uSizePx'),
            dT: gl.getUniformLocation(prog, 'dT'),
        };
    }
};

loadToxicCalcPipe.boarder = {
    fsCode: `#version 300 es
        precision mediump float;

        uniform vec2 border;
        uniform sampler2D uSampler;

        in highp vec2 uv;

        layout(location = 0) out vec4 color;
        void main() {
            float d = texture(uSampler, uv).b;

            if(uv.x < border.x) {
                color.xyw = texture(uSampler, vec2(border.x, uv.y)).xyw * vec3(-1.0, -1.0, 1.0);
            } else if(uv.x > (1.0 - border.x)) {
                color.xyw = texture(uSampler, vec2(1.0 - border.x, uv.y)).xyw * vec3(-1.0, -1.0, 1.0);
            } else if(uv.y < border.y) {
                color.xyw = texture(uSampler, vec2(uv.x, border.y)).xyw * vec3(-1.0, -1.0, 1.0);
            } else if(uv.y > (1.0 - border.y)){
                color.xyw = texture(uSampler, vec2(uv.x, 1.0 - border.y)).xyw * vec3(-1.0, -1.0, 1.0);
            } else {
                color.xyw = texture(uSampler, uv).xyw;
            }
            color.b = d;
        }
    `,
    bindUni: function(gl, prog) {
        return {
            sampler: [gl.getUniformLocation(prog, 'uSampler')],
            border: gl.getUniformLocation(prog, 'border'),
        };
    }

};

loadToxicCalcPipe.addSource = {
    fsCode: `#version 300 es
        precision mediump float;

        uniform sampler2D uSampler;
        uniform sampler2D src;
        uniform float dT;

        in highp vec2 uv;

        layout(location = 0) out vec4 color;
        void main() {
            color = texture(uSampler, uv);
            float s = texture(src, uv).b;
            color.b += s * dT;
            color.a = 0.0;
        }
    `,
    bindUni: function(gl, prog) {
        return {
            sampler: [gl.getUniformLocation(prog, 'uSampler')],
            src: gl.getUniformLocation(prog, 'src'),
            dT: gl.getUniformLocation(prog, 'dT'),
        };
    }
};

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
            vec4 val = texture2D(uSampler, uv);
            gl_FragColor = vec4(uColor, val.b);
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
    params =  {
        vel_x: 0,
        vel_y: 0,
        density: 0,
        preassur: 0,
    };
    constructor(gl, w, h, start, resolution, color = [255, 0, 0]) {
        console.log(gl.getParameter(gl.VERSION));
        this.amtParaTex = Math.ceil(Object.keys(this.params).length / 4.0);
        console.log("layers: " + this.amtParaTex);
        this.w = w * resolution;
        this.h = h * resolution;
        this.color = color.map(function(x) { return x / 255.0; });
        this.gl = gl;

        const setTexPara = function() {
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        }
        const createTextureWS = function(w, h) {
            return function(data) {
                const tex = gl.createTexture();
                console.log(w + " " + h);
                gl.bindTexture(gl.TEXTURE_2D, tex);
                gl.texImage2D(
                    gl.TEXTURE_2D,
                    0,
                    gl.RGBA32F, w, h,
                    0,
                    gl.RGBA, gl.FLOAT, data);
                setTexPara();
                return tex;
            }
        }
        const createTexture = createTextureWS(this.w, this.h);

        this.tSrc = [];
        this.tDst = [];
        this.tSrc[0] = createTexture(start.map(function(x,id){ 
            const trans = [0, 0, 0, 0.5];
            if(x > 1.0) throw "too large";
            return trans[id % 4];
        }));
        this.tDst[0] = createTexture(null);
        for(var i = 1; i < this.amtParaTex; i++) {
            this.tSrc[i] = createTexture(null);
            this.tDst[i] = createTexture(null);
        }

        this.tProduces = createTexture(start.map(function(x, id){
            const trans = [0, 0, x, 0];
            return trans[id % 4];
        }));
        
        this.fbSrc = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbSrc);
        for(var i = 0; i < this.amtParaTex; i++) {
            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + i, gl.TEXTURE_2D, this.tSrc[i], 0);
        }

        this.fbDst = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbDst);
        for(var i = 0; i < this.amtParaTex; i++) {
            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + i, gl.TEXTURE_2D, this.tDst[i], 0);
        }

        gl.bindFramebuffer(gl.FRAMEBUFFER, null);

        this.advectPipe = loadToxicCalcPipe(gl, 'advect');
        this.diffusePipe = loadToxicCalcPipe(gl, 'diffuse');
        this.projectionPipe = loadToxicCalcPipe(gl, 'projection');
        this.forcePipe = loadToxicCalcPipe(gl, 'applyForce');
        this.sourcePipe = loadToxicCalcPipe(gl, 'addSource');
        this.borderPipe = loadToxicCalcPipe(gl, 'boarder');

        // attrLayout for every shader the samer!
        this.computePipe = {
            attrLoc: this.advectPipe.attrLoc,
        };

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
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.tSrc[0]);

        this.gl.uniform3fv(this.renderPipe.uniLoc.color, this.color);
        this.gl.uniformMatrix4fv(this.renderPipe.uniLoc.proj, false, game.camera.proj);

        this.renderBuffer.pos.bindAttribPointer(this.renderPipe.attrLoc.pos, 2, false, 0);
        this.renderBuffer.uv.bindAttribPointer(this.renderPipe.attrLoc.uv, 2, false, 0);
        
        this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

        this.gl.disableVertexAttribArray(this.renderPipe.attrLoc.pos);
        this.gl.disableVertexAttribArray(this.renderPipe.attrLoc.uv);
    }

    swapBuffer() {
        this.tSrc = [this.tDst, this.tDst = this.tSrc][0];
        this.fbSrc = [this.fbDst, this.fbDst = this.fbSrc][0];
    }
    
    bindTextures(pipe) {
        for(var i = 0; i < this.amtParaTex; i++) {
            this.gl.activeTexture(this.gl.TEXTURE0 + i);
            this.gl.uniform1i(pipe.uniLoc.sampler[i], i);
            this.gl.bindTexture(this.gl.TEXTURE_2D, this.tSrc[i]);
        }
    }

    update(dT) {
        if(dT < 0.00001) return;
        this.gl.disable(this.gl.BLEND);
        const viewport = this.gl.getParameter(this.gl.VIEWPORT);
        this.gl.viewport(0, 0, this.w, this.h);

        this.gl.enableVertexAttribArray(this.computePipe.attrLoc.pos);
        this.gl.enableVertexAttribArray(this.computePipe.attrLoc.uv);

        this.computeBuffer.pos.bindAttribPointer(this.computePipe.attrLoc.pos, 2, false, 0);
        this.computeBuffer.uv.bindAttribPointer(this.computePipe.attrLoc.uv, 2, false, 0);

        this.advect(dT);
        this.diffuse(dT, 30);
        this.source(dT);
        // this.addForce();
        this.projection(dT, 80);
        this.applyForce(dT);
        this.bordar();

        this.gl.disableVertexAttribArray(this.computePipe.attrLoc.uv);
        this.gl.disableVertexAttribArray(this.computePipe.attrLoc.pos);

        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
        this.gl.viewport.apply(null, viewport);
        this.gl.enable(this.gl.BLEND);

    }

    advect(dT) {

        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbDst);

        this.gl.useProgram(this.advectPipe.prog);

        this.bindTextures(this.advectPipe);

        this.gl.uniform1f(this.advectPipe.uniLoc.dT, dT);

        this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

        this.swapBuffer();
    }

    source(dT) {
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbDst);

        this.gl.useProgram(this.sourcePipe.prog);

        this.bindTextures(this.sourcePipe);

        this.gl.uniform1f(this.sourcePipe.uniLoc.dT, dT);

        this.gl.activeTexture(this.gl.TEXTURE0 + this.amtParaTex + 0);
        this.gl.uniform1i(this.sourcePipe.uniLoc.src, 1);
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.tProduces);

        this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

        this.swapBuffer();
    }

    diffuse(dT, amt) {
        this.gl.useProgram(this.diffusePipe.prog);

        const alpha = 0.02 /*dx*dx*/ / (/*viscosity*/ 1.0 * dT);
        const beta = 1.0 / (4.0 + alpha);
        for(var i = 0; i < amt; i++) {
            this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbDst);

            this.bindTextures(this.diffusePipe);

            this.gl.uniform2f(this.diffusePipe.uniLoc.size, 1.0 / this.w, 1.0 / this.h);
            this.gl.uniform1f(this.diffusePipe.uniLoc.alpha, alpha);
            this.gl.uniform1f(this.diffusePipe.uniLoc.beta, beta);
            this.gl.uniform1i(this.diffusePipe.uniLoc.layer, 3);

            this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

            this.swapBuffer();
        }
    }

    projection(dT, amt) {
        this.gl.useProgram(this.projectionPipe.prog);

        const alpha =  -1.0; // -(dx*dx)
        for(var i = 0; i < amt; i++) {
            this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbDst);

            this.bindTextures(this.projectionPipe);

            this.gl.uniform1f(this.projectionPipe.uniLoc.alpha, alpha);
            this.gl.uniform2f(this.projectionPipe.uniLoc.size, 1.0 / this.w, 1.0 / this.h);

            this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

            this.swapBuffer();
        }
    }

    applyForce(dT) {
        this.gl.useProgram(this.forcePipe.prog);

        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbDst);

        this.bindTextures(this.forcePipe);

        this.gl.uniform1f(this.forcePipe.uniLoc.dT, dT);
        this.gl.uniform2f(this.forcePipe.uniLoc.size, 1.0 / this.w, 1.0 / this.h);

        this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

        this.swapBuffer();
    }

    bordar() {
        this.gl.useProgram(this.borderPipe.prog);

        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.fbDst);

        this.bindTextures(this.borderPipe);

        this.gl.uniform2f(this.borderPipe.uniLoc.border, 1.5 / this.w, 1.5 / this.h);

        this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

        this.swapBuffer();
    }
};

export function createToxic(gl, w, h, data, color) {
    const resolution = 100;

    const arr = new Float32Array(data.length * resolution * resolution);
    var src = 0, dst = 0;
    for(var y = 0; y < h; y++) {
        for(var x = 0; x < w; x++) {
            const c = [data[src++] / 255.0, data[src++] / 255.0, data[src++] / 255.0, data[src++] / 255.0];
            for(var by = 0; by < resolution; by ++) {
                dst = ((y * resolution + by) * w * resolution + x * resolution) * 4;
                for(var bx = 0; bx < resolution; bx ++) {
                    for(var i = 0; i < 4; i ++) {
                        arr[dst++] = c[i];
                    }
                }
            }
        }
    }
    console.log(arr);
    return new Toxic(gl, w*2, h*2, arr, resolution / 2.0, color);
}