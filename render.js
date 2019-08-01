function loadShader(gl, _type, _src) {
	const shader = gl.createShader(_type);
	gl.shaderSource(shader, _src);
	gl.compileShader(shader);
	if(!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		console.error('failed to compileShader: ' + gl.getShaderInfoLog(shader));
		return null;
	}
	return shader;
}


function loadProgram(gl, _vsCode, _fsCode) {
	const vs = loadShader(gl, gl.VERTEX_SHADER, _vsCode);
	const fs = loadShader(gl, gl.FRAGMENT_SHADER, _fsCode);

	if(!vs || !fs) {
		console.error('can\'t build program without shader');
		return null;
	}
	const prog = gl.createProgram();
	gl.attachShader(prog, vs);
	gl.attachShader(prog, fs);
	gl.linkProgram(prog);
	if(!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
		console.error('unable to link program: ' + gl.getProgramLog(prog));
		return null;
	}
	return prog;
}

function loadTex(gl, url) {
	const tex = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, tex);
	const lvl = 0;
	const inFormat = gl.RGBA;
	const w = 1;
	const h = 1;
	const border = 0;
	const srcFormat = gl.RGBA;
	const srcType = gl.UNSIGNED_BYTE;
	const pixel = new Uint8Array([0,0,255,255]);
	gl.texImage2D(gl.TEXTURE_2D, lvl, inFormat, w, h, border, srcFormat, srcType, pixel);
	const img = new Image();
	img.onload = function() {
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.texImage2D(gl.TEXTURE_2D, lvl, inFormat, srcFormat, srcType, img);
		const wI = image.width;
		const hI = image.heaght;
		if((wI & (wI - 1)) === 0 && (hI & (hI - 1)) === 0) {
			gl.generateMipmap(gl.TEXTURE_2D);
		} else {
			gl.texParameter(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAPM_TO_EDGE);
			gl.texParameter(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			gl.texParameter(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
		}
	};
	img.src = url;
	return tex;
}

function setupClear(gl, clear,color,depth) {
	if (clear === undefined) clear = gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT; 
	if (color === undefined) color = [0.2, 0.3, 0.1, 1];
	if (depth === undefined) depth = 1.0;
	gl.clearColor(color[0], color[1], color[2], color[3]);
	gl.clearDepth(depth);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	
	gl.clear(clear);
}

function initStaticArrayBuffer(gl, data, type) {
	const buffer = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
	gl.bufferData(gl.ARRAY_BUFFER, new type(data), gl.STATIC_DRAW);
	return buffer;
}
	
function buildVannilaPipe(gl) {
// @beg=glsl@
	const vsSrc=`
attribute vec4 aVecPos;
attribute vec2 aTexCord;

uniform mat4 uView;
uniform mat4 uProj;

varying highp vec2 vTexCord;

void main(void) {
	gl_Position = uProj * uView * aVecPos;
	vTexCord = aTexCord;
}
`;
	const fsSrc=`
varying highp vec2 vTexCord;
uniform sampler2D uSampler;

void main(void) {
	gl_FragColor = texture2D(uSampler, vTexCord);
}
`;
// @end=glsl@

	const prog = loadProgram(gl, vsSrc, fsSrc);
	if (!prog) {
		console.error('failed to load Prgogram');
		return null;
	}
	return prog; 
}

function buildScene(gl) {
	const prog = buildVannilaPipe(gl);
	setupClear(gl, undefined, [0.2, 0.3, 0.2, 1], 1.1);
	const drawCalls = {
		gl,
		vanilla: prog,
		player: createPlayer(gl)
	};
}

var numPlayers = 0;
function createPlayer(gl) {
	numPlayers += 1;
	if (numPlayers > 1) console.war('more then one player is constructed');
	const player = {};
	const anitmations = []; // {frames: int, name: 'name'}
	player.buffers = {}; //{pos: undefined; tex: undefined;};
	player.view = undefined; //matrix
	// init wasm player
	return player;
}
