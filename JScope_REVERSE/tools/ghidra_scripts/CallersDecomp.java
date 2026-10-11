// @category JScopeRE
// Usage: CallersDecomp.java <fnNameSubstr[,fnNameSubstr...]> <outFileName> [depth] [maxFuncs]
// Finds functions whose name contains any given substring, walks up `depth` caller levels,
// and decompiles everything found.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class CallersDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        String[] keys = a[0].split(",");
        String outName = a[1];
        int depth = a.length > 2 ? Integer.parseInt(a[2]) : 1;
        int maxFuncs = a.length > 3 ? Integer.parseInt(a[3]) : 60;

        String outDir = System.getenv("JSCOPE_OUT");
        if (outDir == null) outDir = "D:\\OpenScope\\JScope_REVERSE\\dumps";

        FunctionManager fm = currentProgram.getFunctionManager();
        ReferenceManager rm = currentProgram.getReferenceManager();

        // seeds
        Set<Function> seeds = new LinkedHashSet<>();
        for (Function f : fm.getFunctions(true)) {
            for (String k : keys) {
                if (f.getName().contains(k)) { seeds.add(f); break; }
            }
        }
        Set<Function> level = new LinkedHashSet<>(seeds);
        Set<Function> all = new LinkedHashSet<>(seeds);
        Map<Function,Integer> lvlOf = new HashMap<>();
        for (Function f : seeds) lvlOf.put(f, 0);
        for (int d = 1; d <= depth; d++) {
            Set<Function> next = new LinkedHashSet<>();
            for (Function f : level) {
                for (Reference r : rm.getReferencesTo(f.getEntryPoint())) {
                    Function c = fm.getFunctionContaining(r.getFromAddress());
                    if (c == null || all.contains(c)) continue;
                    next.add(c);
                }
            }
            for (Function f : next) { lvlOf.put(f, d); all.add(f); }
            level = next;
            if (level.isEmpty()) break;
        }

        PrintWriter w = new PrintWriter(new FileWriter(new File(outDir, outName)));
        w.println("=== seeds: " + keys.length + " keys -> " + seeds.size() + " fns; total with callers(depth " + depth + "): " + all.size() + " ===");
        for (Function f : all) w.println("  L" + lvlOf.get(f) + "  " + f.getName() + " @ " + f.getEntryPoint() + " size=" + f.getBody().getNumAddresses());

        DecompInterface ifc = new DecompInterface();
        ifc.openProgram(currentProgram);
        int done = 0;
        for (Function f : all) {
            if (done++ >= maxFuncs) { w.println("\n[truncated at " + maxFuncs + "]"); break; }
            w.println("\n\n################ L" + lvlOf.get(f) + " " + f.getName() + " @ " + f.getEntryPoint() + " ################");
            DecompileResults res = ifc.decompileFunction(f, 120, monitor);
            if (res != null && res.decompileCompleted()) w.println(res.getDecompiledFunction().getC());
            else w.println("// decompile failed");
        }
        w.close(); ifc.dispose();
        println("wrote " + outName + " total=" + all.size() + " decompiled=" + Math.min(done, maxFuncs));
    }
}
