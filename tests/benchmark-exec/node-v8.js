var fs = require('fs');
var path = '/Users/hanka/Documents/export-medium.csv';
var pattern = 'ABCD|DEFGH|EFGHI|A{4,}';

process.argv.forEach((val, index, array) => {
	if (val.substring(0,5) == "path=") path = val.substring(5);
	if (val.substring(0,8) == "pattern=") pattern = val.substring(8);
});

console.log(path);

var readline = require('readline');
var stream = require('stream');
var instream = fs.createReadStream(path);
var outstream = new stream();
var rl = readline.createInterface(instream, outstream);

var lines = [];

console.time("reading input file");
rl.on('line', function(line) {
	lines.push(line);
})


rl.on('close', function() {
	console.timeEnd("reading input file");
	var re = new RegExp(pattern);

	var count = 0;
	console.time("matching");
	for (i in lines) {
		var line = lines[i];
		//console.log(line.search(re));
		if (line.search(re) != -1) {
			//console.log(line);
			count++; //console.log(line);
		}
	}
	console.timeEnd("matching");

	console.log("matched: "+count);
});

