// @category JScopeRE
// Renames thunk functions after the API-name string they resolve via GetProcAddress.
// Usage: RenameByStringRef.java <stringRegex>
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.util.*;
import java.util.regex.*;

public class RenameByStringRef extends GhidraScript {
    @Override
    public void run() throws Exception {
        String regex = getScriptArgs().length > 0 ? getScriptArgs()[0] : "^JLINK";
        Pattern pat = Pattern.compile(regex);
        Listing lst = currentProgram.getListing();
        FunctionManager fm = currentProgram.getFunctionManager();
        ReferenceManager rm = currentProgram.getReferenceManager();

        Map<Function, String> map = new LinkedHashMap<>();
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
                if (f.getBody().getNumAddresses() > 400) continue;  // thunks only
                map.putIfAbsent(f, s);
            }
        }
        int n = 0;
        for (Map.Entry<Function, String> e : map.entrySet()) {
            try {
                e.getKey().setName(e.getValue(), SourceType.USER_DEFINED);
                println("renamed " + e.getKey().getEntryPoint() + " -> " + e.getValue());
                n++;
            } catch (Exception ex) {
                println("skip " + e.getKey().getEntryPoint() + ": " + ex.getMessage());
            }
        }
        println("RENAMED " + n);
    }
}
