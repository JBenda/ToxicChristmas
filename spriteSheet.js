import { loadTextureFromImg, Buffer } from './render.js';

export default class SpriteSheet {
    /**
     * @param w width per sprite
     * @param h heigt per sprite
     */
    constructor(gl, img, w, h, flipped = false) {
        this.gl = gl;
        this.tex = loadTextureFromImg(gl, img);
        this.w = Math.floor(img.width / w);
        this.h = Math.floor(img.height / h);
        this.img = img;
        this.flipped = flipped;

        this.spriteW = w / img.width;
        this.spriteH = h / img.height;
        const sw = this.spriteW;
        const sh = this.spriteH;

        function toUVW(n) {
            return n * sw;
        }
        function toUVH(n) {
            return n * sh;
        }

        this.uvs = new Array(this.h * this.w * 4 * 2);
        var pos = 0;
        for(var y = 0; y < this.h; y++) {
            for(var x = 0; x < this.w; x++) {
                this.uvs[pos] = toUVW(x + 0.01); pos++;
                this.uvs[pos] = toUVH(y + 0.99); pos++;
                this.uvs[pos] = toUVW(x + 0.99); pos++;
                this.uvs[pos] = toUVH(y + 0.99); pos++;
                this.uvs[pos] = toUVW(x + 0.01); pos++;
                this.uvs[pos] = toUVH(y + 0.01); pos++;
                this.uvs[pos] = toUVW(x + 0.99); pos++;
                this.uvs[pos] = toUVH(y + 0.01); pos++;
            }
        }

        if(flipped) {
            for(var y = 0; y < this.h; y++) {
                for(var x = 0; x < this.w; x++) {
                    this.uvs[pos] = toUVW(x + 0.99); pos++;
                    this.uvs[pos] = toUVH(y + 0.99); pos++;
                    this.uvs[pos] = toUVW(x + 0.01); pos++;
                    this.uvs[pos] = toUVH(y + 0.99); pos++;
                    this.uvs[pos] = toUVW(x + 0.99); pos++;
                    this.uvs[pos] = toUVH(y + 0.01); pos++;
                    this.uvs[pos] = toUVW(x + 0.01); pos++;
                    this.uvs[pos] = toUVH(y + 0.01); pos++;
                }
            }
        }

        this.buffer = new Buffer(gl, this.uvs, Float32Array, gl.ARRAY_BUFFER, gl.STATIC_DRAW);
    }
    // uvs for triangle list
    getUVS(x,y, flipped) {
        const id = y === undefined ? x : y * this.w + x;
        if(flipped) {
            if(this.flipped) {
                const offset = this.w * this.h * 8;
                return this.uvs.slice(id * 8 + offset, id * 8 + 6 + offset)
                    .concat(this.uvs.slice(id*8 + 2 + offset, id*8+8 + offset))
            }
            throw "this sheet is not flippable";
        }
        return this.uvs.slice(id * 8, id * 8 + 6).concat(this.uvs.slice(id*8 + 2, id*8+8));
    }
    spriteOffset(x,y, flipped) {
        var id = x;
        if (y !== undefined) id = x + y * this.w;

        if(flipped) {
            if(this.flipped) {
                return id * 4 * 2 * 4 + this.w * this.h * 4 * 2 * 4;
            }
            throw "this sprite sheet is not flippable";
        }
        return id * 4 * 2 * 4;
    }

    bindVertexPointerForSprite(attrLoc, x,y, flipped) {
        this.buffer.bindAttribPointer(attrLoc, 2, false, this.spriteOffset(x,y, flipped));
    }
}
