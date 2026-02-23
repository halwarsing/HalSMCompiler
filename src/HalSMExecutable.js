var Module=Module||{};

Module=Object.assign({
    preRun: [],
    postRun: [],
    print: function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        console.log(text);
    },
    canvas:null,
    ctx:null,
    setStatus: null,
    printErr:function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        console.error(text);
        
    },
    input_text:function(text) {
        return prompt(text);
    },
    is_load_module:false,
    HalSMVariableType_int:0,
    HalSMVariableType_double:1,
    HalSMVariableType_char:2,
    HalSMVariableType_void:3,
    HalSMVariableType_HalSMArray:4,
    HalSMVariableType_str:5,
    HalSMVariableType_int_array:6,
    HalSMVariableType_HalSMFunctionC:7,
    HalSMVariableType_HalSMClassC:8,
    HalSMVariableType_HalSMRunClassC:9,
    HalSMVariableType_HalSMSetArg:10,
    HalSMVariableType_HalSMError:11,
    HalSMVariableType_HalSMNull:12,
    HalSMVariableType_HalSMRunFunc:13,
    HalSMVariableType_HalSMRunFuncC:14,
    HalSMVariableType_HalSMLocalFunction:15,
    HalSMVariableType_HalSMCModule:16,
    HalSMVariableType_HalSMModule:17,
    HalSMVariableType_HalSMCompiler:18,
    HalSMVariableType_HalSMRunClass:19,
    HalSMVariableType_HalSMDoubleGet:20,
    HalSMVariableType_HalSMClass:21,
    HalSMVariableType_HalSMVar:22,
    HalSMVariableType_HalSMMult:23,
    HalSMVariableType_HalSMDivide:24,
    HalSMVariableType_HalSMPlus:25,
    HalSMVariableType_HalSMMinus:26,
    HalSMVariableType_HalSMEqual:27,
    HalSMVariableType_HalSMNotEqual:28,
    HalSMVariableType_HalSMMore:29,
    HalSMVariableType_HalSMLess:30,
    HalSMVariableType_HalSMBool:31,
    HalSMVariableType_HalSMDict:32,
    HalSMVariableType_HalSMSetVar:33,
    HalSMVariableType_HalSMReturn:34,
    HalSMVariableType_HalSMFunctionCTypeDef:35,
    HalSMVariableType_HalSMFunctionArray:36,
    HalSMVariableType_unsigned_int:37,
    HalSMVariableType_custom:38,
    HalSMVariableType_HalSMMoreEqual:39,
    HalSMVariableType_HalSMLessEqual:40,
    HalSMVariableType_HalSMShift:41,
    HalSMVariableType_HalSMPow:42,
    HalSMVariableType_HalSMModulo:43,
    HalSMVariableType_HalSMAND:44,
    HalSMVariableType_HalSMOR:45,
    HalSMVariableType_HalSMNegate:46,
    HalSMVariableType_HalSMNegativeSign:47,
    //0.1.5
    HalSMVariableType_HalSMGOTO:48,
    HalSMVariableType_HalSMGOTOSector:49,
    HalSMVariableType_HalSMInlineIf:50, //if(a>b) 1 else 0
    HalSMVariableType_HalSMInlineElse:51,
    HalSMVariableType_HalSMBreak:52,
    HalSMVariableType_HalSMXOR:53,
    HalSMVariableType_HalSMBAND:54,
    HalSMVariableType_HalSMBOR:55,
    HalSMVariableType_HalSMBNOT:56,
},Module);

Module['onRuntimeInitialized']=function() {
    Module.ccall("init","undefined",[],[],{async:true}).then(()=>{
        Module.null=Module.ccall("getHalSMNull","number",[],[],{async:false});
        if (Module['onLoad']){Module['onLoad']();}
    });
}

function runHSME(index) {
    Module.ccall("run","undefined",["number"],[index],{async:true});
}

function loadHSME(name) {
    Module.is_load_module=true;
    return new Promise((resolve)=>{
        var out=Module.ccall("load","number",["string"],[name],{async:true});
        var interval;
        interval=setInterval(function(){if(Module.is_load_module===false){clearInterval(interval);resolve(out);}},1);
    });
}

function loadWebAssemblyModuleHSME(binary, flags, libName, localScope, handle) {
    var metadata = getDylinkMetadata(binary);
    currentModuleWeakSymbols = metadata.weakImports;
    function loadModule() {
        var firstLoad = !handle || !HEAP8[handle + 8 >>> 0];
        if (firstLoad) {
            var memAlign = Math.pow(2, metadata.memoryAlign);
            var memoryBase = metadata.memorySize ? alignMemory(getMemory(metadata.memorySize + memAlign), memAlign) : 0;
            var tableBase = metadata.tableSize ? wasmTable.length : 0;
            if (handle) {
                HEAP8[handle + 8 >>> 0] = 1;
                HEAPU32[handle + 12 >>> 2] = memoryBase;
                HEAP32[handle + 16 >>> 2] = metadata.memorySize;
                HEAPU32[handle + 20 >>> 2] = tableBase;
                HEAP32[handle + 24 >>> 2] = metadata.tableSize
            }
        } else {
            memoryBase = HEAPU32[handle + 12 >>> 2];
            tableBase = HEAPU32[handle + 20 >>> 2]
        }
        var tableGrowthNeeded = tableBase + metadata.tableSize - wasmTable.length;
        if (tableGrowthNeeded > 0) {
            wasmTable.grow(tableGrowthNeeded)
        }
        var moduleExports;
        function resolveSymbol(sym) {
            var resolved = resolveGlobalSymbol(sym).sym;
            if (!resolved && localScope) {
                resolved = localScope[sym]
            }
            if (!resolved) {
                resolved = moduleExports[sym]
            }
            return resolved
        }
        var proxyHandler = {
            get(stubs, prop) {
                switch (prop) {
                case "__memory_base":
                    return memoryBase;
                case "__table_base":
                    return tableBase
                }
                if (prop in wasmImports && !wasmImports[prop].stub) {
                    return wasmImports[prop]
                }
                if (!(prop in stubs)) {
                    var resolved;
                    stubs[prop] = function() {
                        if (!resolved)
                            resolved = resolveSymbol(prop);
                        return resolved.apply(null, arguments)
                    }
                }
                return stubs[prop]
            }
        };
        var proxy = new Proxy({},proxyHandler);
        var info = {
            "GOT.mem": new Proxy({},GOTHandler),
            "GOT.func": new Proxy({},GOTHandler),
            "env": proxy,
            "wasi_snapshot_preview1": proxy
        };
        function postInstantiation(module, instance) {
            updateTableMap(tableBase, metadata.tableSize);
            moduleExports = relocateExports(instance.exports, memoryBase);
            moduleExports = Asyncify.instrumentWasmExports(moduleExports);
            if (!flags.allowUndefined) {
                reportUndefinedSymbols()
            }
            function addEmAsm(addr, body) {
                var args = [];
                var arity = 0;
                for (; arity < 16; arity++) {
                    if (body.indexOf("$" + arity) != -1) {
                        args.push("$" + arity)
                    } else {
                        break
                    }
                }
                args = args.join(",");
                var func = `(${args}) => { ${body} };`;
                ASM_CONSTS[start] = eval(func)
            }
            if ("__start_em_asm"in moduleExports) {
                var start = moduleExports["__start_em_asm"];
                var stop = moduleExports["__stop_em_asm"];
                while (start < stop) {
                    var jsString = UTF8ToString(start);
                    addEmAsm(start, jsString);
                    start = HEAPU8.indexOf(0, start) + 1
                }
            }
            function addEmJs(name, cSig, body) {
                var jsArgs = [];
                cSig = cSig.slice(1, -1);
                if (cSig != "void") {
                    cSig = cSig.split(",");
                    for (var i in cSig) {
                        var jsArg = cSig[i].split(" ").pop();
                        jsArgs.push(jsArg.replace("*", ""))
                    }
                }
                var func = `(${jsArgs}) => ${body};`;
                moduleExports[name] = eval(func)
            }
            for (var name in moduleExports) {
                if (name.startsWith("__em_js__")) {
                    var start = moduleExports[name];
                    var jsString = UTF8ToString(start);
                    var parts = jsString.split("<::>");
                    addEmJs(name.replace("__em_js__", ""), parts[0], parts[1]);
                    delete moduleExports[name]
                }
            }
            var applyRelocs = moduleExports["__wasm_apply_data_relocs"];
            if (applyRelocs) {
                if (runtimeInitialized) {
                    applyRelocs()
                } else {
                    __RELOC_FUNCS__.push(applyRelocs)
                }
            }
            var init = moduleExports["__wasm_call_ctors"];
            if (init) {
                if (runtimeInitialized) {
                    init()
                } else {
                    __ATINIT__.push(init)
                }
            }
            return moduleExports
        }
        if (flags.loadAsync) {
            if (binary instanceof WebAssembly.Module) {
                var instance = new WebAssembly.Instance(binary,info);
                return Promise.resolve(postInstantiation(binary, instance))
            }
            return WebAssembly.instantiate(binary, info).then(result=>postInstantiation(result.module, result.instance))
        }
        var module = binary instanceof WebAssembly.Module ? binary : new WebAssembly.Module(binary);
        var instance = new WebAssembly.Instance(module,info);
        return postInstantiation(module, instance)
    }
    return loadModule()
}

function loadDynamicLibraryHSME(libData,libName, flags={
    global: true,
    nodelete: true
}, localScope, handle) {
    var dso = LDSO.loadedLibsByName[libName];
    if (dso) {
        if (flags.global && !dso.global) {
            dso.global = true;
            if (dso.exports !== "loading") {
                mergeLibSymbols(dso.exports, libName)
            }
        }
        if (flags.nodelete && dso.refcount !== Infinity) {
            dso.refcount = Infinity
        }
        dso.refcount++;
        if (handle) {
            LDSO.loadedLibsByHandle[handle] = dso
        }
        return flags.loadAsync ? Promise.resolve(true) : true
    }
    dso = newDSO(libName, handle, "loading");
    dso.refcount = flags.nodelete ? Infinity : 1;
    dso.global = flags.global;
    function getExports() {
        var preloaded = preloadedWasm[libName];
        if (preloaded) {
            return flags.loadAsync ? Promise.resolve(preloaded) : preloaded
        }
        if (flags.loadAsync) {
            return loadWebAssemblyModuleHSME(libData, flags, libName, localScope, handle)
        }
        return loadWebAssemblyModuleHSME(libData, flags, libName, localScope, handle)
    }
    function moduleLoaded(exports) {
        if (dso.global) {
            mergeLibSymbols(exports, libName)
        } else if (localScope) {
            Object.assign(localScope, exports)
        }
        dso.exports = exports
    }
    if (flags.loadAsync) {
        return getExports().then(exports=>{
            moduleLoaded(exports);
            return true
        }
        )
    }
    moduleLoaded(getExports());
    return true;
}

function addModuleHSME(m) {
    var outModule=Module.ccall("initModule","number",["string"],[m['name']]);
    var outClass,outFunc;
    for (const [key,value] of Object.entries(m['classes'])) {
        outClass=Module.ccall("initClassC","number",["string"],[key]);
        for (const [keyFunc,valueFunc] of Object.entries(value['functions'])) {
            outFunc=Module.ccall("initFunctionC","number",["number"],[Module.addFunction(valueFunc,"iiii")]);
            Module.ccall("addFunctionToClassC","undefined",["number","string","number"],[outClass,keyFunc,outFunc]);
        }
        for (const [keyVar,valueVar] of Object.entries(value['variables'])) {
            Module.ccall("addVariableToClassC","undefined",["number","string","number"],[outClass,keyVar,valueVar]);
        }
        Module.ccall("addClassCToModule","undefined",["number","number"],[outModule,outClass]);
    }

    for (const [key,value] of Object.entries(m['functions'])) {
        outFunc=Module.ccall("initFunctionC","number",["number"],[Module.addFunction(value,"iiii")]);
        Module.ccall("addFunctionToModule","undefined",["number","string","number"],[outModule,key,outFunc]);
    }

    for (const [key,value] of Object.entries(m['variables'])) {
        Module.ccall("addVariableToModule","undefined",["number","string","number"],[outModule,key,value]);
    }
    Module.ccall("addModule","undefined",["number"],[outModule]);
}

function GetTypeVariable(variable) {
    return Module.ccall("getTypeVariable","number",["number"],[variable]);
}

function HalSMArrayToArray(args) {
    var size=Module.ccall("getSizeHalSMArray","number",["number"],[args]);
    var out=[],val;
    for (var i=0;i<size;i++) {
        val=Module.ccall("getVariableFromHalSMArray","number",["number","number"],[args,i]);
        out.push(val);
    }
    return out;
}

function DictToObject(dict) {
    var size=Module.ccall("getSizeDict","number",["number"],[dict]);
    var out={},val,key,elem;
    for (var i=0;i<size;i++) {
        elem=Module.ccall("getDictElementFromDict","number",["number","number"],[dict,i]);
        val=Module.ccall("getValueFromDictElement","number",["number"],[elem]);
        key=Module.ccall("getKeyFromDictElement","number",["number"],[elem]);
        out[key]=val;
    }
    return out;
}

function ArrayToHalSMArray(args) {
    var out=Module.ccall("initHalSMArray","number",[],[]);
    for (var i=0;i<args.length;i++) {
        Module.ccall("appendToHalSMArray","undefined",["number","number"],[out,args[i]]);
    }
    return out;
}

function ObjectToDict(obj) {
    var out=Module.ccall("initDict","number",[],[]);
    for (const [key,value] of Object.entries(obj)) {
        Module.ccall("putToDict","undefined",["number","number"],[out,Module.ccall("initDictElement","number",["number","number"],[key,value])]);
    }
    return out;
}

function GetValueFromVariable(variable) {
    return Module.ccall("getValueVariable","number",["number"],[variable]);
}

function GetStringFromValue(val) {
    return Module.ccall("getStringFromValue","string",["number"],[val]);
}

function GetIntFromValue(val) {
    return parseInt(Module.ccall("getIntFromValue","string",["number"],[val]));
}

function GetFloatFromValue(val) {
    return parseFloat(Module.ccall("getDoubleFromValue","string",["number"],[val]));
}

function GetCharFromValue(val) {
    return Module.ccall("getCharFromValue","number",["number"],[val]);
}

function GetUnsignedIntFromValue(val) {
    return Module.ccall("getUnsignedIntFromValue","number",["number"],[val]);
}

function GetStringFromVariable(variable) {
    return GetStringFromValue(GetValueFromVariable(variable));
}

function GetIntFromVariable(variable) {
    return GetIntFromValue(GetValueFromVariable(variable));
}

function GetFloatFromVariable(variable) {
    return GetFloatFromValue(GetValueFromVariable(variable));
}

function GetCharFromVariable(variable) {
    return GetCharFromValue(GetValueFromVariable(variable));
}

function GetUnsignedIntFromVariable(variable) {
    return GetUnsignedIntFromValue(GetValueFromVariable(variable));
}

function VariableInit(value,type) {
    return Module.ccall("variableInit","number",["number","number"],[value,type]);
}

function StringToValue(str) {
    return Module.ccall("stringToValue","number",["string"],[str]);
}

function IntToValue(n) {
    return Module.ccall("intToValue","number",["number"],[n]);
}

function FloatToValue(n) {
    return Module.ccall("floatToValue","number",["string"],[n.toString()]);
}

function CharToValue(n) {
    return Module.ccall("charToValue","number",["number"],[n]);
}

function UnsignedIntToValue(n) {
    return Module.ccall("unsignedIntToValue","number",["number"],[n]);
}

function StringToVariable(str) {
    return VariableInit(StringToValue(str),Module.HalSMVariableType_str);
}

function IntToVariable(n) {
    return VariableInit(IntToValue(n),Module.HalSMVariableType_int);
}

function FloatToVariable(n) {
    return VariableInit(FloatToValue(n),Module.HalSMVariableType_double);
}

function CharToVariable(n) {
    return VariableInit(CharToValue(n),Module.HalSMVariableType_char);
}

function UnsignedIntToVariable(n) {
    return VariableInit(UnsignedIntToValue(n),Module.HalSMVariableType_unsigned_int);
}