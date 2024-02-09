const mfy = import('minify');
const fs = require('node:fs');
const fsAsync = require('node:fs/promises');

const rootdir = './EspUnoWifi/';
const datadir  = rootdir + 'data/';
const outputFile = 'resources.h';
const files = [
    'pendant.html',
    'serial.html',
    'serial.js',
    'netstat.html',
    'redir.html',
    'index.html'
]

var origSize = 0;
var minifiedSize = 0;

async function main() {

    console.log(`Compiling ${files.length} files...\n`);
    const minify = (await mfy).minify;

    if(fs.existsSync(rootdir+outputFile)){
        await fsAsync.truncate(rootdir+outputFile);
    }
    var ws = fs.createWriteStream(rootdir+outputFile, {});

    ws.write(`
/////////////////////////////////////////////////////
//   GENERATED FILE!                               //
//   Do not edit!                                  //
//   Generate using CLI: npm run build             //
/////////////////////////////////////////////////////
#ifndef __HTML_H__
#define __HTML_H__
`);

    for(var file of files){
        const hndl = await fsAsync.stat(datadir+file);
        const mini = await minify(datadir+file, {js:{compress: {drop_console: true}}});

        const so = hndl.size;
        const sm = mini.length;

        console.info(`Minified ${file} (${so} -> ${sm}) bytes`)
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

#endif`);
    ws.close();

    console.log(`\n${files.length} files compiled into ${outputFile}\nTotal: ${origSize} -> ${minifiedSize} bytes\n`)
};

main();
