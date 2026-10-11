// @category JScopeRE
// Usage: XrefDecomp.java <stringRegex> <outFileName> [maxFuncs]
// Finds strings matching regex, resolves referencing functions, decompiles them.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.address.*;
import ghidra.program.util.*;
import java.io.*;
import java.util.*;
import java.util.regex.*;

public class XrefDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String regex = args[0];
        String outName = args[1];
        int maxFuncs = args.length > 2 ? Integer.parseInt(args[2]) : 40;

        String outDir = System.getenv("JSCOPE_OUT");
        if (outDir == null) outDir = "D:\\OpenScope\\JScope_REVERSE\\dumps";

        Pattern pat = Pattern.compile(regex);
        Listing lst = currentProgram.getListing();
        FunctionManager fm = currentProgram.getFunctionManager();
        ReferenceManager rm = currentProgram.getReferenceManager();

        // map function -> set of matching strings
        Map<Function, Set<String>> hits = new LinkedHashMap<>();
        Map<Function, Set<Address>> hitAddr = new LinkedHashMap<>();

        DataIterator di = lst.getDefinedData(true);
        while (di.hasNext()) {
            Data d = di.next();
            Object v = d.getValue();
            if (!(v instanceof String)) continue;
            String s = (String) v;
            if (!pat.matcher(s).find()) continue;
            for (Reference r : rm.getReferencesTo(d.getAddress())) {
                Function f = fm.getFunctionContaining(r.getFromAddress());
                if (f == null) continue;
                hits.computeIfAbsent(f, k -> new LinkedHashSet<>()).add(s);
                hitAddr.computeIfAbsent(f, k -> new LinkedHashSet<>()).add(d.getAddress());
            }
        }

        PrintWriter w = new PrintWriter(new FileWriter(new File(outDir, outName)));
        w.println("=== strings matching /" + regex + "/ referenced by " + hits.size() + " functions ===");
        for (Map.Entry<Function, Set<String>> e : hits.entrySet()) {
            w.println("\n--- " + e.getKey().getName() + " @ " + e.getKey().getEntryPoint()
                    + "  size=" + e.getKey().getBody().getNumAddresses());
            for (String s : e.getValue()) w.println("      \"" + s.replace("\n", "\\n") + "\"");
        }

        DecompInterface ifc = new DecompInterface();
        ifc.openProgram(currentProgram);
        int done = 0;
        for (Function f : hits.keySet()) {
            if (done++ >= maxFuncs) { w.println("\n[truncated at " + maxFuncs + " functions]"); break; }
            w.println("\n\n################ DECOMP: " + f.getName() + " @ " + f.getEntryPoint() + " ################");
            DecompileResults res = ifc.decompileFunction(f, 90, monitor);
            if (res != null && res.decompileCompleted()) {
                w.println(res.getDecompiledFunction().getC());
            } else {
                w.println("// decompile failed: " + (res == null ? "null" : res.getErrorMessage()));
            }
        }
        w.close();
        ifc.dispose();
        println("wrote " + outName + " : " + hits.size() + " funcs, decompiled " + Math.min(done, maxFuncs));
    }
}
