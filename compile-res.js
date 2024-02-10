const mfy = import('minify');
const fs = require('node:fs');
const fsAsync = require('node:fs/promises');
const process = require('node:process');

var origSize = 0;
var minifiedSize = 0;

const config = require('./resources.json');
const outputFile = config.rootdir + config.outputFile;
const files = config.files;
const datadir = config.rootdir + config.datadir;

const DBG = !process.argv.every(a => a != '-debug');
if (DBG) {
    console.log("--- DEBUG ---");
}


async function removeDebug(file) {
    console.log(` - Removing debug stuff from ${file}`); 
    var content = await fsAsync.readFile(datadir+file, {encoding: 'utf-8'});
    pattern = /\/\* debug \*\/((?:.|\n)*?)(?:\/\* else \*\/((?:.|\n)*?))?\/\* end \*\//gm;

    if (pattern.test(content)){
        
        content = content.replace(pattern, DBG ? "$1" : "$2");
        
        const path = datadir+'release.'+file;
        await fsAsync.writeFile(path, content);
        return path;
    }
    return datadir + file;
}

async function main() {
    console.log(`Compiling ${config.files.length} files...\n`);

    const minify = (await mfy).minify;

    if(fs.existsSync(outputFile)){
        await fsAsync.truncate(outputFile);
    }
    var ws = fs.createWriteStream(outputFile, {});

    ws.write(`
/////////////////////////////////////////////////////
//   GENERATED FILE!                               //
//   Do not edit!                                  //
//   Generate using CLI: npm run build             //
/////////////////////////////////////////////////////
#ifndef __RESOURCES_H__
#define __RESOURCES_H__

`);

    for(var file of files){
        
        console.info(`Minifying ${file}`)
        var path = datadir+file;
            path = await removeDebug(file);
        const hndl = await fsAsync.stat(path);
        const mini = await minify(path, {js:{compress: {drop_console: true}}});

        const so = hndl.size;
        const sm = mini.length;

        console.info(` - Done (${so} -> ${sm}) bytes`)
        origSize += so;
        minifiedSize += sm;

        ws.write(`
// file:${file.padStart(24, ' ')}
// size:            ${so.toFixed().padStart(12, ' ')} bytes
// compressed size: ${sm.toFixed().padStart(12, ' ')} bytes

const char res_${file.replace('.', '_')}[] PROGMEM = R"=====(
`);

        ws.write(mini)
        ws.write(`
)=====";

`);

    }
    
    ws.write(`
// Total:  ${origSize} -> ${minifiedSize} bytes
#endif`);
    ws.close();

    console.log(`\n${files.length} files compiled into ${outputFile}\nTotal: ${origSize} -> ${minifiedSize} bytes\n`)
};

main();
