function loadShader(gl, _type, _src) {
	const shader = gl.createShader(_type);
	gl.shaderSource(shader, _src);
	gl.compileShader(shader);
	if(!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		console.err('failed to compileShader: ' + gl.getShaderInfoLog(shader));
		return null;
	}
	return shader;
}

function loadProgram(gl, _vsCode, _fsCode) {
	const vs = loadShader(gl, gl.VERTEX_SHADER, _vsCode);
	const fs = loadShader(gl, gl.FRAGMENT_SHADER, _fsCode);

	if(!vs || !fs) {
		console.err('can\'t build program without shader');
		return null;
	}
	const prog = gl.createProgram();
	gl.attachShader(prog, vs);
	gl.attachShader(prog, fs);
	gl.linkProgram(prog);
	if(!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
		console.err('unable to link program: ' + gl.getProgramLog(prog));
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


