// @category JScopeRE
// Usage: DecompAt.java <outFileName> <0xADDR[,0xADDR...]>
// For each address: disassemble, create a function if missing, decompile, dump to $JSCOPE_OUT/<out>.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;

public class DecompAt extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String outName = args[0];
        String[] addrs = args[1].split(",");
        String outDir = System.getenv("JSCOPE_OUT");
        if (outDir == null) outDir = "D:\\OpenScope\\JScope_REVERSE\\dumps";

        Listing lst = currentProgram.getListing();
        FunctionManager fm = currentProgram.getFunctionManager();
        PrintWriter w = new PrintWriter(new FileWriter(new File(outDir, outName)));

        DecompInterface ifc = new DecompInterface();
        ifc.openProgram(currentProgram);

        for (String a : addrs) {
            Address addr = toAddr(a.trim());
            w.println("\n################ " + addr + " ################");

            // disassemble a window so the listing has instructions even if not analyzed
            disassemble(addr);
            Function f = fm.getFunctionContaining(addr);
            if (f == null || !f.getEntryPoint().equals(addr)) {
                f = createFunction(addr, null);
                if (f == null) {
                    // fall back: create at the exact address ignoring existing body overlap
                    try {
                        f = createFunction(addr, "FUN_" + addr.toString().substring(2));
                    } catch (Exception e) { /* ignore */ }
                }
            }
            if (f == null) { w.println("// could not create function"); continue; }
            w.println("// " + f.getName() + " @ " + f.getEntryPoint() + " size=" + f.getBody().getNumAddresses());
            for (Function c : f.getCallingFunctions(monitor)) w.println("// CALLER: " + c.getName() + " @ " + c.getEntryPoint());
            for (Function c : f.getCalledFunctions(monitor)) w.println("// CALLEE: " + c.getName() + " @ " + c.getEntryPoint());

            DecompileResults res = ifc.decompileFunction(f, 90, monitor);
            if (res != null && res.decompileCompleted()) {
                w.println(res.getDecompiledFunction().getC());
            } else {
                w.println("// decompile failed: " + (res == null ? "null" : res.getErrorMessage()));
                // dump raw instructions as fallback
                InstructionIterator it = lst.getInstructions(f.getBody(), true);
                int n = 0;
                while (it.hasNext() && n++ < 400) {
                    Instruction i = it.next();
                    w.println("   " + i.getAddress() + "  " + i.toString());
                }
            }
        }
        w.close();
        ifc.dispose();
        println("wrote " + outName);
    }
}
