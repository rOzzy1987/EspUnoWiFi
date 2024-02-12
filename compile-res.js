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
const tempdir = config.rootdir + config.tempdir;
const outdir = config.rootdir + config.outdir;


async function preprocess(file, args) {
    console.log(` - Preprocessing ${file}`); 
    var content = await fsAsync.readFile(datadir + file, {encoding: 'utf-8'});

    const pattern = /\/\* if ([a-z]+) \*\/((?:.|\n)*?)(?:\/\* else \*\/((?:.|\n)*?))?\/\* end \*\//gm;
    var match;
    var changed = false;
    
    while(match = pattern.exec(content))
    {
        content = content.replace(pattern, args.indexOf(match[1]) ? "$1" : "$2");
        changed = true;
    }
    if(changed) {
        const path = tempdir + file;
        await fsAsync.writeFile(path, content);
        return path;
    }
    return datadir + file;
}

async function main(args) {
    console.log(`Compiling ${config.files.length} files...\n`);
    args = args.slice(2);
    console.log(`Flags used:\n   ${args.join('\n   ')}\n\n`);

    const minify = (await mfy).minify;

    // if(fs.existsSync(outputFile)){
    //     await fsAsync.truncate(outputFile);
    // }
    var ws = fs.createWriteStream(outputFile);

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
        var path = await preprocess(file, args);
        const hndl = await fsAsync.stat(path);
        const mini = await minify(path, {js:{compress: {drop_console: true}}});

        const so = hndl.size;
        const sm = mini.length;

        console.info(` - Done (${so} -> ${sm}) bytes`)
        origSize += so;
        minifiedSize += sm;

        
        await fsAsync.writeFile(outdir+file, mini);

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

main(process.argv);
