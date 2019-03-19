
var path = '/Users/hanka/Documents/documents/export-medium.csv';
var pattern = '[0-9a-fA-F]{8,16}?';

if (typeof arguments !== 'undefined') {
	for (i in arguments) {
		var val = arguments[i];
		if (val.substring(0,5) == "path=") path = val.substring(5);
		if (val.substring(0,8) == "pattern=") pattern = val.substring(8);
	}
} else if (typeof process !== 'undefined') {
	process.argv.forEach((val, index, array) => {
		if (val.substring(0,5) == "path=") path = val.substring(5);
		if (val.substring(0,8) == "pattern=") pattern = val.substring(8);
	});
}


//console.log(path);

//console.time("reading input file");
var buffer = null;
if (readFile) {
	buffer = readFile(path);
} else {
	var fs = require('fs');
	buffer = fs.readFileSync(path);
}
//console.timeEnd("reading input file");
var lines = [];
//console.time("to string");
var str = buffer.toString('ascii');
//console.timeEnd("to string");
//console.time("split");
var data = str.split("\n");
//console.timeEnd("split");

// read lines to array

var re = new RegExp(pattern);

var count = 0;
print(preciseTime());
var start = preciseTime();
//console.time("matching");
for (i in data) {
	var line = data[i];
	////console.log(line.search(re));
	if (line.search(re) != -1) {
		//print(line+'\n');
		////console.log(line);
		count++; ////console.log(line);
	}
}
var end = preciseTime();
print("matching: " + ((end-start)*1000) + " ms");
print("matched: " + count);
//console.timeEnd("matching");

//console.log("matched: "+count);
