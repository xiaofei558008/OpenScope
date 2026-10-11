// @category JScopeRE
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.address.*;
import java.io.*;
import java.util.*;

public class DumpOverview extends GhidraScript {
    @Override
    public void run() throws Exception {
        String outDir = System.getenv("JSCOPE_OUT");
        if (outDir == null) outDir = "D:\\OpenScope\\JScope_REVERSE\\dumps";
        new File(outDir).mkdirs();

        // 1. Function inventory
        FunctionManager fm = currentProgram.getFunctionManager();
        PrintWriter fw = new PrintWriter(new FileWriter(new File(outDir, "functions.txt")));
        int n = 0;
        for (Function f : fm.getFunctions(true)) {
            fw.printf("%s\t%d\t%s%n", f.getEntryPoint(), f.getBody().getNumAddresses(), f.getName());
            n++;
        }
        fw.close();
        println("functions: " + n);

        // 2. All defined strings with address
        PrintWriter sw = new PrintWriter(new FileWriter(new File(outDir, "defined_strings.txt")));
        Listing lst = currentProgram.getListing();
        DataIterator di = lst.getDefinedData(true);
        int sn = 0;
        while (di.hasNext()) {
            Data d = di.next();
            if (d.getValue() instanceof String) {
                String v = (String) d.getValue();
                if (v.length() >= 3) {
                    sw.printf("%s\t%d\t%s%n", d.getAddress(), v.length(), v.replace("\n", "\\n").replace("\r", ""));
                    sn++;
                }
            }
        }
        sw.close();
        println("strings: " + sn);
        println("DONE");
    }
}
