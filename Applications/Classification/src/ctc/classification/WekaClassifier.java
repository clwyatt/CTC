package ctc.classification;

import weka.classifiers.Classifier;
import weka.classifiers.Evaluation;
import weka.core.converters.ConverterUtils.DataSource;
import weka.core.Instances;

import java.io.ObjectInputStream;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;

public class WekaClassifier {
	public String buildClassifier(String name,
								String fname_train,
								String fname_model) throws Exception
	{
		ArrayList<String> options = new ArrayList<String>();
		options.add("-t");
		options.add(fname_train);
		options.add("-c");
		options.add("last");
		options.add("-d");
		options.add(fname_model);
		// output statistics only, not the classifier
		options.add("-o");  
		// Outputs detailed information-retrieval statistics for each class.
		options.add("-i");  
		return Evaluation.evaluateModel(name, options.toArray(new String[1]));
	}

	public CTCEvaluation evaluateData(String fname_model,
									String fname_test) throws Exception
	{
		// build Classifier object
		InputStream is = new FileInputStream(fname_model);
		ObjectInputStream objis = new ObjectInputStream(is);
		Classifier classifier = (Classifier) objis.readObject();
		objis.close();

		// build Evaluation object
		DataSource ds_test = new DataSource(fname_test); 
		Instances insts = ds_test.getDataSet();
		Instances insts_clone = new Instances(insts);
		
		insts_clone.setClassIndex(insts.numAttributes() - 1);
		CTCEvaluation eval = new CTCEvaluation(insts_clone);
		
		// evaluate
		eval.evaluate(classifier, insts_clone);
		
		return eval;
	}

	
	/**
	 * @param args
	 */
	public static void main(String[] args) 
	{
		try {
			String name = "weka.classifiers.functions.MultilayerPerceptron";
			String fname_train = 
				"/gpfs0/santagoGrp/data/WFU_prior_CTC_data/Classification/" +
				"ExtractedPolyps2.mat.matched.2.csv.balanced.0.5.arff";
			String fname_model = 
				"/gpfs0/santagoGrp/data/WFU_prior_CTC_data/Classification/" +
				"model.weka";
			String fname_test = 
				"/gpfs0/santagoGrp/data/WFU_prior_CTC_data/Classification/" +
				"ExtractedPolyps2.mat.matched.2.arff";
			
			WekaClassifier weka = new WekaClassifier();
			
			weka.buildClassifier(name, fname_train, fname_model);
	
			weka.evaluateData(fname_model, fname_test);
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

}
