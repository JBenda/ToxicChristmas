
function CretaeSrpite(w, h, x, y) {
	const bl = {x: x * w, y: y * h}; // bottomLeft
	return {
		corners: [bl.x,bl.y, bl.x+w,bl.y, bl.x+w,bl.y+h, bl.x,bl.y+h],
		getCorners: function(order) {
			if (order === undefined) return corners;
			return order.reduce(function(arr, val){arr.append(this.corners.slice(val, val+2))}, []);
		}
	};
}

function SpriteSheet(url, collums, rows, amuont) {
	if (amount === undefined) amuont = collums * rows;
	const w = 1.0 / collums;
	const h = 1.0 / rows;
	const sheet = {
		AddAnimation: function(name, spriteList) {AddAnimation(this, name, spriteList);},
		url,
		data: new Array(amount),
		animations: {},
		get: function(x,y){
			if (x >= collums || y >= rows) return null;
			return data[x + y * collums];
		},
		collums,
		rows
	}
	sheet.data.forEach(function(el, i, arr){
		arr[i] = CretaeSrpite(w, h, Math.trunc(i/collums), i % collums);
	});
}

// sprite list as [[x,y], [x,y]]...
function AddAnimation(sheet, name, spriteList) {
	if (sheet.animations[name] !== undefined) console.warning('overwrite animation: ' + name);
	sheet.animations[name] = {
		frames: spriteList.length,
		sprites: spriteList,
		textData: function(order) {
			this.sprites.reduce(function(arr, val){ arr.append(super.get(...val).getCorners(order));}, []);
		}
	};
}
