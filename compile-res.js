const mfy = import('minify');
const fs = require('node:fs');
const fsAsync = require('node:fs/promises');
const process = require('node:process');

var origSize = 0;
var minifiedSize = 0;

const config = require('./resources.json');
const outputFile = config.outputFile;
const files = config.files;
const datadir = config.workdir + config.datadir;
const tempdir = config.workdir + config.tempdir;
const outdir = config.workdir + config.outdir;


function getFilePath(file) {
    var parts = file.split('.');
    if (parts[parts.length-1] == "js"){
        return datadir + file;
    }
    return config.sourcedir + file;
}

async function preprocess(file, args) {
    console.log(` - Preprocessing ${file}`); 
    var path = getFilePath(file);
    var content = await fsAsync.readFile(path, {encoding: 'utf-8'});
    var changed = 0;
    do {
        var match = /\/\* if ([a-z]+) \*\/((?:.|\n|\r)*?)(?:\/\* else \*\/((?:.|\n|\r)*?))?\/\* endif \*\//gm.exec(content);
        if(!match) break;
        content = content.replace(match[0], match[args.indexOf(match[1]) > -1 ? 2 : 3]);
        changed++;
    } while (true);

    if(changed) {
        console.log(`    ${changed} blocks processed`);
        path = tempdir + file;
        await fsAsync.writeFile(path, content);
    }
    return path;
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
