class Scene{
    constructor (context) {
        this.gl = context;
    }

    clear() {
        this.gl.clear(this.gl.DEPTH_BUFFER_BIT | this.gl.COLOR_BUFFER_BIT);
    }
};

function powO2(num) {
    return (num & (num - 1)) == 0;
}
/** 
 * @param _defColor color until texture is loaded (rgba 0-255) 
 * @param _setParams function(image) with will be called after tex is loaded and binded to set parameters
*/
export function loadTexture(gl, _url, _setParams, _defColor) {
    const tex = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, tex);

    const px = new Uint8Array(_defColor !== undefined ? _defColor : [0, 255, 0, 255]);
    gl.texImage2D(gl.TEXTURE_2D,
        0,
        gl.RGBA,
        1,
        1,
        0,
        gl.RGBA,
        gl.UNSIGNED_BYTE,
        px);

    const img = new Image();
    img.onload = function() {
        gl.bindTexture(gl.TEXTURE_2D, tex);
        gl.texImage2D(gl.TEXTURE_2D,
            0,
            gl.RGBA,
            gl.RGBA,
            gl.UNSIGNED_BYTE,
            img);
        if(powO2(img.width) && posO2(img.height)) {
            gl.generateMipmap(gl.TEXTURE_2D);
        }
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST_MIPMAP_NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        if(_setParams) _setParams(img);
        devNull.appendChild(img);
    }
    img.src = _url;
    return tex;
}

export function loadTextureFromImg(gl, img) {
    const tex = gl.createTexture();
    const px = new Uint8Array([0, 0, 255, 255]);
    function load() {
        console.log("load :) ");
        gl.bindTexture(gl.TEXTURE_2D, tex);
        if(img.complete) {
            gl.texImage2D(gl.TEXTURE_2D,
                0,
                gl.RGBA,
                gl.RGBA,
                gl.UNSIGNED_BYTE,
                img);
            } else {
            gl.texImage2D(gl.TEXTURE_2D,
                0,
                gl.RGBA,
                1,
                1,
                0,
                gl.RGBA,
                gl.UNSIGNED_BYTE,
                px);
        }
        if(powO2(img.width) && powO2(img.height)) {
            gl.generateMipmap(gl.TEXTURE_2D);
            // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST_MIPMAP_NEAREST);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        } else {
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        }
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    }
    load();

    img.onload = load();

    return tex;
}

export function buildScene(gl) {
    gl.clearColor(0.2, 0.4, 0.7, 1);
    gl.clearDepth(1);
    
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

    return new Scene(gl);
}

function loadShader(gl, _type, _code) {
    const shader = gl.createShader(_type);
    gl.shaderSource(shader, _code);
    gl.compileShader(shader);
    if(!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.error('failed to compile Shader' + gl.getShaderInfoLog(shader));
        return null;
    }
    return shader;
}

export function loadProgram(gl, _vsC, _fsC) {
    const vs = loadShader(gl, gl.VERTEX_SHADER, _vsC);
    const fs = loadShader(gl, gl.FRAGMENT_SHADER, _fsC);

    const prog = gl.createProgram();
    gl.attachShader(prog, vs);
    gl.attachShader(prog, fs);
    gl.linkProgram(prog);

    if(!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
        console.error('Unable to link Program: ' + gl.getProgramLog(prog));
        return null;
    }
    return prog;
}

function loadNormalPipe(gl) {
    if(loadNormalPipe.pipe === undefined) {
        loadNormalPipe.pipe = {};
        const vsCode =`
        uniform mat4 umView;
        uniform mat4 umProj;

        attribute vec2 aPos;
        attribute vec2 aUV;

        varying lowp vec2 uv;

        void main() {
            gl_Position = umProj * umView * vec4(aPos, 0, 1);
            uv = aUV;
        }
        `;
        const fsCode = `
        uniform sampler2D uSampler;

        varying lowp vec2 uv;

        void main() {
            gl_FragColor = texture2D(uSampler, uv);
        }
        `

        loadNormalPipe.pipe.prog = loadProgram(gl, vsCode, fsCode);
        loadNormalPipe.pipe.attrLoc = {
            pos: gl.getAttribLocation(loadNormalPipe.pipe.prog, 'aPos'),
            uv: gl.getAttribLocation(loadNormalPipe.pipe.prog, 'aUV'),
        };
        loadNormalPipe.pipe.uniLoc = {
            sampler: gl.getUniformLocation(loadNormalPipe.pipe.prog, 'uSampler'),
            mView: gl.getUniformLocation(loadNormalPipe.pipe.prog, 'umView'),
            mProj: gl.getUniformLocation(loadNormalPipe.pipe.prog, 'umProj'),
        };
    }
    return loadNormalPipe.pipe;
}

export class Buffer {
    constructor(gl, data, dataType, bufferType, drawType) {
        this.gl = gl;
        this.buffer = gl.createBuffer();
        this.bufferType = bufferType;
        this.dataType = dataType;
        this.drawType = drawType;
        switch(dataType) {
            case Float32Array: this.type = gl.FLOAT; break;
            case Uint32Array: this.type = gl.UINT; break;
            default: console.error('undefined buffer data type: ' + dataType);
        }
        gl.bindBuffer(bufferType, this.buffer);
        gl.bufferData(bufferType,
            new dataType(data),
            drawType);
    }
    set(data) {
        this.gl.bindBuffer(this.bufferType, this.buffer);
        this.gl.bufferData(this.bufferType,
            new this.dataType(data),
            this.drawType);
    }
    loadFromHeap(array) {
        this.gl.bindBuffer(this.bufferType, this.buffer);
        this.gl.bufferData(this.bufferType,
                array,
                this.drawType, );
    }
    bindAttribPointer(attrLoc, size, normelized, offset) {
        this.gl.bindBuffer(this.bufferType, this.buffer);
        this.gl.vertexAttribPointer(
            attrLoc,
            size,
            this.type,
            normelized,
            0,
            offset
        );
    }
}

class Player {
    constructor(context, _spriteSheet) {
        this.gl = context;
        
        this.pipe = loadNormalPipe(context);

        const planeP = [-1,1, 1,1, -1,-1, 1,-1];
        const pos = new Buffer(context, planeP, Float32Array, this.gl.ARRAY_BUFFER, this.gl.STATIC_DRAW);
        
        this.buffer = {pos, uv: _spriteSheet.buffer};
        
        // this.sprite = loadTextureFromImg(this.gl, document.getElementById('runner'));//loadTexture(gl, "assets/runner.jpg");
        this.spriteSheet = _spriteSheet;
    }
    SetViewMatrix(mat) {
        this.mView = mat;
    }
    draw(game) {
        this.gl.useProgram(this.pipe.prog);
        this.gl.enableVertexAttribArray(this.pipe.attrLoc.pos);
        this.gl.enableVertexAttribArray(this.pipe.attrLoc.uv)

        this.gl.activeTexture(this.gl.TEXTURE0);
        this.gl.uniform1i(this.pipe.uniLoc.sampler, 0);
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.spriteSheet.tex);
        this.spriteSheet.bindVertexPointerForSprite(this.pipe.attrLoc.uv, this.frame[0]);

        this.gl.uniformMatrix4fv(this.pipe.uniLoc.mView, false, this.mView);
        this.gl.uniformMatrix4fv(this.pipe.uniLoc.mProj, false, game.camera.proj);

        this.buffer.pos.bindAttribPointer(this.pipe.attrLoc.pos, 2, false, 0);
        
        this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);

        this.gl.disableVertexAttribArray(this.pipe.attrLoc.pos);
        this.gl.disableVertexAttribArray(this.pipe.attrLoc.uv);
    }
};

export function createPlayer(gl, _spriteSheet)  {
    return new Player(gl, _spriteSheet);
}
