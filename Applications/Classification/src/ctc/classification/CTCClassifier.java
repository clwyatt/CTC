package ctc.classification;

import java.io.File;
import java.io.FileReader;
import java.util.Properties;

import weka.filters.unsupervised.attribute.Remove;
import weka.filters.Filter;

public class CTCClassifier {
	protected WekaClassifier m_weka;

	public CTCClassifier()
	{
		m_weka = new WekaClassifier();
	}
	
	public String buildClassifier(String name,
						  		String fname_train,
						  		String fname_model) throws Exception
	{
		fname_train = _removeUnwantedFeatures(fname_train);
		return m_weka.buildClassifier(name, fname_train, fname_model);
	}
	
	/**
	 * 
	 * @param fname_model
	 * @param fname_test
	 * @return 
	 * @throws Exception
	 */
	public CTCEvaluation evaluateData(String fname_model, String fname_test) 
							throws Exception
	{
		fname_test = _removeUnwantedFeatures(fname_test);
		return m_weka.evaluateData(fname_model, fname_test);
	}
	
	/**
	 * Remove features: polypID, coordinates X,Y,Z
	 * @param fname
	 * @return A filename point to a temporary file
	 */
	protected String _removeUnwantedFeatures(String fname) 
						throws Exception
	{
		String fname_tmp = 
			fname.substring(fname.lastIndexOf(File.separatorChar)) 
									+ ".R1234.arff";
		fname_tmp = System.getProperty("java.io.tmpdir") + fname_tmp;
		String[] options = new String[6];
		options[0] = "-i";
		options[1] = fname;
		options[2] = "-o";
		options[3] = fname_tmp;
		options[4] = "-R";
		Properties prop = new Properties();
		prop.load(new FileReader("ctc.classification.properties"));
		options[5] = prop.getProperty("remove_feature", "");
		
		if (options[5].length() != 0)
			Filter.filterFile(new Remove(), options);
		else
			fname_tmp = fname;

		return fname_tmp;
	}

	public static void main(String[] args)
	{
		
	}

}
