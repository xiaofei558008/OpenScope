// @category JScopeRE
// Usage: RefsTo.java <hexaddr[,hexaddr...]> <outFileName>
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class RefsTo extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        String[] addrs = a[0].split(",");
        String outName = a[1];
        String outDir = System.getenv("JSCOPE_OUT");
        if (outDir == null) outDir = "D:\\OpenScope\\JScope_REVERSE\\dumps";

        FunctionManager fm = currentProgram.getFunctionManager();
        ReferenceManager rm = currentProgram.getReferenceManager();
        PrintWriter w = new PrintWriter(new FileWriter(new File(outDir, outName)));
        Set<Function> funcs = new LinkedHashSet<>();
        for (String s : addrs) {
            Address t = currentProgram.getAddressFactory().getDefaultAddressSpace()
                    .getAddress(Long.parseLong(s.trim(), 16));
            w.println("=== refs to " + t + " ===");
            for (Reference r : rm.getReferencesTo(t)) {
                Function f = fm.getFunctionContaining(r.getFromAddress());
                w.printf("   from %s  [%s]  in %s%n", r.getFromAddress(), r.getReferenceType(),
                        f == null ? "<none>" : f.getName() + "@" + f.getEntryPoint());
                if (f != null) funcs.add(f);
            }
        }
        DecompInterface ifc = new DecompInterface();
        ifc.openProgram(currentProgram);
        for (Function f : funcs) {
            w.println("\n\n################ " + f.getName() + " @ " + f.getEntryPoint() + " ################");
            DecompileResults res = ifc.decompileFunction(f, 120, monitor);
            if (res != null && res.decompileCompleted()) w.println(res.getDecompiledFunction().getC());
            else w.println("// decompile failed");
        }
        w.close(); ifc.dispose();
        println("wrote " + outName + " funcs=" + funcs.size());
    }
}
