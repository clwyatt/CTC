package ctc.classification;

public class CTCResultReporter {

	public String mergePredicts(String[] ids,
								String[] truelabels, 
								String[][] predicts,
								String[] name_classifiers)
	{
		assert(predicts.length > 0);
		
		String csv = new String();
		String endl = System.getProperty("line.separator");
		csv = "ID" + "," + "TRUE_LABEL" + ",";
		for (int idx = 0; idx < name_classifiers.length; idx++)
			csv += name_classifiers[idx] + ",";
		csv = csv.substring(0, csv.length() - 1); // remove the last ","
		csv = csv + endl;
		
		for (int idx = 0; idx < ids.length; idx++)
		{
			csv += ids[idx] + "," + truelabels[idx] + ",";
			for (int idy = 0; idy < predicts.length; idy++)
				csv += predicts[idy][idx] + ",";
			csv = csv.substring(0, csv.length() - 1); // remove the last ","
			csv = csv + endl;
		}
		
		return csv;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		CTCResultReporter rr = new CTCResultReporter();
		
		// test mergePredictions
		String[] ids = 
					{"1", "2", "3", "4", "5" };
		String[] truelabels = 
					{"T", "T", "F", "T", "F" };
		String[][] predicts = 
					{ 	{"T", "T", "T", "T", "T"}, 
						{"F", "F", "F", "F", "F"},
						{"T", "F", "T", "F", "F"},
						{"F", "F", "T", "F", "T"}
					};
		String[] name_classifiers = 
					{"cls1", "cls2", "cls3", "cls4" };
		
		String csv = 
			rr.mergePredicts(ids, truelabels, predicts, name_classifiers);
		
		System.out.println(csv);
	}

}
