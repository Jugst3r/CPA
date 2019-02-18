
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.HashMap;

public class ReadFile {

	public static void main(String[] args) {
		int cpt = 0;
		HashMap<Integer, Integer> rename_points = new HashMap<>();
		BufferedReader br = null;
		FileReader fr = null;
		Writer writer = null;

		try {

			// br = new BufferedReader(new FileReader(FILENAME));
			fr = new FileReader(args[0]);
			br = new BufferedReader(fr);
			
			writer = new BufferedWriter(new OutputStreamWriter(
		              new FileOutputStream("filename.txt"), "utf-8"));
			String sCurrentLine;

			while ((sCurrentLine = br.readLine()) != null) {
				sCurrentLine = sCurrentLine.replaceAll("#[^\n]*", "");
				//System.out.println(sCurrentLine);
				try {
					String[] parts = sCurrentLine.split("\\s+");
					int p1 = Integer.parseInt(parts[0]);
					int p2 = Integer.parseInt(parts[1]);
					
					if(rename_points.containsKey(p1)) {
						p1 = rename_points.get(p1);
					}
					else {
						rename_points.put(p1, cpt);
						p1 = cpt++;
					}
					if(rename_points.containsKey(p2)) {
						p1 = rename_points.get(p2);
					}
					else {
						rename_points.put(p2, cpt);
						p2 = cpt++;
					}
					writer.write(p1 + " " +p2+"\n");
					
				}
				catch(NumberFormatException e) {
					System.out.println("was unabled to read int");
					continue;
				}
			}

		} catch (IOException e) {

			e.printStackTrace();

		} finally {

			try {

				if (br != null)
					br.close();

				if (fr != null)
					fr.close();
				
				if(writer != null)
					writer.close();
				

			} catch (IOException ex) {

				ex.printStackTrace();

			}

		}

	}

}
