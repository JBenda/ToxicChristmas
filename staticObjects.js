import { loadProgram, Buffer } from './render.js';

function loadStaicObjectPipe(gl) {
    if(loadStaicObjectPipe.pipe === undefined) {
        loadStaicObjectPipe.pipe = {};
        const vsCode = `
        uniform mat4 umProj;
        uniform float ufTime;
        uniform float ufSpriteWidth;
        
        attribute vec2 aPos;
        attribute vec4 aAnimation;
        // [0]: uv x
        // [1]: uv y
        // [2]: frameTime
        // [3]: frames

        varying lowp vec2 uv;

        void main() {
            gl_Position = umProj * vec4(aPos, 0, 1);
            float offset = mod(aAnimation[3], 1.0);
            float frames = floor(aAnimation[3]);
            uv = 
                   mod(floor((ufTime + offset) / aAnimation[2]), frames)  
                *  vec2(ufSpriteWidth, 0)
                +  vec2(aAnimation[0], aAnimation[1]);
        }
        `;
        const fsCode = `
        uniform sampler2D uSampler;
        
        varying lowp vec2 uv;

        void main() {
            gl_FragColor = texture2D(uSampler, uv);
        }
        `;

        loadStaicObjectPipe.pipe.prog = loadProgram(gl, vsCode, fsCode);
        const prog = loadStaicObjectPipe.pipe.prog;
        loadStaicObjectPipe.pipe.attrLoc = {
            pos: gl.getAttribLocation(prog, 'aPos'),
            animation: gl.getAttribLocation(prog, 'aAnimation'),
        };
        loadStaicObjectPipe.pipe.uniLoc = {
            sampler: gl.getUniformLocation(prog, 'uSampler'),
            mProj: gl.getUniformLocation(prog, 'umProj'),
            time: gl.getUniformLocation(prog, 'ufTime'),
            spriteW: gl.getUniformLocation(prog, 'ufSpriteWidth'),
        };
    }
    return loadStaicObjectPipe.pipe;
}

class StaticObjects {
    constructor(gl, _spriteSheet) {
        this.gl = gl;
        this.spriteSheet = _spriteSheet;

        this.pipe = loadStaicObjectPipe(gl);

        this.buffer = {
            pos: new Buffer(gl, [], Float32Array, gl.ARRAY_BUFFER, gl.STATIC_DRAW),
            animation: new Buffer(gl, [], Float32Array, gl.ARRAY_BUFFER, gl.STATIC_DRAW),
        };

        this.count = 0;
    }

    draw(game) {
        this.gl.useProgram(this.pipe.prog);
        this.gl.enableVertexAttribArray(this.pipe.attrLoc.pos);
        this.gl.enableVertexAttribArray(this.pipe.attrLoc.animation);

        this.gl.activeTexture(this.gl.TEXTURE0);
        this.gl.uniform1i(this.pipe.uniLoc.sampler, 0);
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.spriteSheet.tex);

        this.gl.uniformMatrix4fv(this.pipe.uniLoc.mProj, false, game.camera.proj);
        this.gl.uniform1f(this.pipe.uniLoc.time, game.time.s % 100);
        this.gl.uniform1f(this.pipe.uniLoc.spriteW, this.spriteSheet.spriteW);

        this.buffer.animation.bindAttribPointer(this.pipe.attrLoc.animation, 4, false, 0);
        this.buffer.pos.bindAttribPointer(this.pipe.attrLoc.pos, 2, false, 0);

        this.gl.drawArrays(this.gl.TRIANGLES, 0, this.count * 6);

        this.gl.disableVertexAttribArray(this.pipe.attrLoc.pos);
        this.gl.disableVertexAttribArray(this.pipe.attrLoc.animation);
    }

    fetchObjs(wasmFn, heap) {
        this.count = wasmFn.staticObjectsNumber();

        const planeP = [-1,1, 1,1, -1,-1, 1,1, -1,-1, 1,-1];
        const pos = new Float32Array(heap, wasmFn.staticObjectsPos() ,this.count * 2);
        
        const p = new Array(this.count * 12).fill(0).map(
            function(_,i) {
                return planeP[i % 12] + pos[Math.floor((i / 12)) * 2 + (i % 2)];
            }
        )
        this.buffer.pos.set(p);
        const sheet = this.spriteSheet;
        const uvs = Array.from(new Uint32Array(heap, wasmFn.staticObjectsSpriteIds(), this.count)).reduce(
            function(arr, el) {
                return arr.concat(sheet.getUVS(el));
            },[]
        );
        const animations = new Float32Array(heap, wasmFn.staticObjectsAnimation(), this.count * 3);
        const anim =    new Array(this.count * 24).fill(0).map(
            function(_,i) {
                const objId = Math.floor(i / 24);
                const vertexId = Math.floor(i / 4);
                const numId = i % 4;
                return numId < 2
                    ? uvs[vertexId * 2 + (numId % 2)] 
                    : (numId === 2
                        ? animations[objId * 3]
                        : animations[objId * 3 + 1] + animations[objId * 3 + 2]);
            }
        )
        this.buffer.animation.set(anim);
        return this;
    }
};

export function createStaticObjects(gl, _spriteSheet, wasmFn, heap) {
    return (new StaticObjects(gl, _spriteSheet)).fetchObjs(wasmFn, heap);
}
