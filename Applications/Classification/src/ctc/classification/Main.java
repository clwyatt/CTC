package ctc.classification;

import java.io.File;
import java.io.FileWriter;
import java.io.FileReader;
import java.io.BufferedWriter;
import java.util.ArrayList;
import java.util.Properties;
import java.util.StringTokenizer;
import java.text.DateFormat;
import java.util.Date;

import weka.core.Instances;
import weka.core.converters.ConverterUtils.DataSource;

public class Main {

	public void buildClassifier(String[] names,
						  		String fname_train,
						  		String[] fname_models,
						  		String fname_log) throws Exception
	{
		CTCClassifier ctcclassifier = new CTCClassifier();
		String log = new String();
		
		for (int idx = 0; idx < names.length; idx++)
		{
			String message = ctcclassifier.buildClassifier(names[idx],
														 fname_train,
														 fname_models[idx]);
			
			log += "=========================================================";
			log += System.getProperty("line.separator");
			log += names[idx];
			log += System.getProperty("line.separator");
			log += message;
			log += System.getProperty("line.separator");
		}
		
		// output log 
		if (fname_log.length() == 0)
			fname_log = System.getProperty("java.io.tmpdir") 
								+ File.separatorChar + "train.log";
		BufferedWriter writer = new BufferedWriter(new FileWriter(fname_log));
		writer.write(log);
		writer.close();
	}
	
	public void evaluateData(String[] fname_models,
							 String fname_test,
							 String fname_predict,
							 String fname_log) throws Exception
	{
		// get ids & true_labels for each data
		DataSource ds_test = new DataSource(fname_test); 
		Instances insts = ds_test.getDataSet();
		insts.setClassIndex(insts.numAttributes() - 1);

		String[] ids = new String[insts.numInstances()];
		String[] truelabels = new String[insts.numInstances()];
		for (int idx = 0; idx < insts.numInstances(); idx++)
		{
			ids[idx] = insts.instance(idx).toString(0);
			truelabels[idx] = insts.instance(idx).toString(insts.classIndex());
		}

		// get predicts for each model
		String[] name_classifiers = new String[fname_models.length];
		String[][] predicts = 
			new String[fname_models.length][insts.numInstances()];

		String log = new String();
		CTCClassifier ctcclassifier = new CTCClassifier();
		for (int idx = 0; idx < fname_models.length; idx++)
		{
			CTCEvaluation eval = 
				ctcclassifier.evaluateData(fname_models[idx], fname_test);
			predicts[idx] = eval.getPredicts();
			name_classifiers[idx] = fname_models[idx].substring(
										fname_models[idx].lastIndexOf('.') + 1);

			log += "=========================================================";
			log += System.getProperty("line.separator");
			log += name_classifiers[idx];
			log += System.getProperty("line.separator");
			log += eval.toSummaryString();
			log += System.getProperty("line.separator");
			log += eval.toClassDetailsString();
			log += System.getProperty("line.separator");
			log += eval.toMatrixString();
			log += System.getProperty("line.separator");
		}

		// output the predictions
		CTCResultReporter ctcrr = new CTCResultReporter();
		String csv = ctcrr.mergePredicts(ids, truelabels, predicts, 
										 name_classifiers);
		FileWriter of = new FileWriter(fname_predict);
		of.write(csv);
		of.close();
		
		// output log 
		if (fname_log.length() == 0)
			fname_log = System.getProperty("java.io.tmpdir") + 
									File.separatorChar + "test.log";
		BufferedWriter writer = new BufferedWriter(new FileWriter(fname_log));
		writer.write(log);
		writer.close();
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			Properties prop = new Properties();
			prop.load(new FileReader("ctc.classification.properties"));
			String dir_rep = prop.getProperty("dir_repositpry");
			String dir_output = prop.getProperty("dir_output", 
										System.getProperty("java.io.tmpdir"));
			String fname_models_prefix = prop.getProperty("model_file_prefix");
			String fname_predict = prop.getProperty("predict_file",
													"predict.csv");
			String fname_log_train = prop.getProperty("logfile_train",
													  "train.log");
			String fname_log_test = prop.getProperty("logfile_test",
													 "test.log");
			double dist = Double.valueOf(prop.getProperty("dist"));
			double train_percent = Double.valueOf(
										prop.getProperty("train_percent"));
			boolean use_all_as_test_data = Boolean.valueOf(
									prop.getProperty("use_all_as_test_data"));
			String str_classifiers = prop.getProperty("classifiers");
			Boolean generate_train_test = Boolean.valueOf(
								prop.getProperty("generate_train_test_file"));
			String fname_train_data = prop.getProperty("file_train");
			String fname_test_data = prop.getProperty("file_test");
		
			CTCData.RESAMPLE_SCHEME resample_scheme;
			String resample = prop.getProperty("resample_scheme");
			if (resample.toUpperCase().equals("OVERSAMPLE"))
				resample_scheme = CTCData.RESAMPLE_SCHEME.OVERSAMPLE;
			else if (resample.toUpperCase().equals("UNDERSAMPLE"))
				resample_scheme = CTCData.RESAMPLE_SCHEME.UNDERSAMPLE;
			else if (resample.toUpperCase().equals("SMOTE"))
				resample_scheme = CTCData.RESAMPLE_SCHEME.SMOTE;
			else
				throw new Exception("invalid 'resample_scheme': " + resample);

			// parse classifier names
			ArrayList<String> names = new ArrayList<String>();
			StringTokenizer st = new StringTokenizer(str_classifiers, ";");
			while (st.hasMoreTokens())
				names.add(st.nextToken());

			if (dir_rep.length() == 0 || dir_output.length() == 0)
				throw new IllegalArgumentException("Invalid repository: "
						+ dir_rep + " or output dir: " + dir_output);
			
			if (dir_output.charAt(dir_output.length()-1) != File.separatorChar)
				dir_output += File.separatorChar;
			
            // generate train/test data
			// mkdir to hold re-sampled data, train/test data, and model files 
			String dir_experiment = dir_output 
							+ DateFormat.getDateTimeInstance().format(new Date()) 
							+ File.separatorChar;
			if (new File(dir_experiment).mkdir() == false)
				throw new Exception("mkdir failed: " + dir_experiment);
			System.out.println("Experiment result stored in: \n\t" 
								+ dir_experiment);

			if (generate_train_test)
			{
				CTCData ctcdata = new CTCData();
				String fname_merge = ctcdata.mergeRepository(dir_rep, 
															 dir_experiment);
				ctcdata.generateTrainTestFile(fname_merge, 
											  dir_experiment,
											  train_percent);
				ctcdata.resample(ctcdata.getTrainFilename(), 
								 dir_experiment,
								 resample_scheme,
								 dist);
				//dir_output = rep.generateTrainTestData(resample_scheme, dist, train_percent);
				fname_train_data = ctcdata.getTrainFilename();
				fname_test_data = ctcdata.getTestFilename();
				if (use_all_as_test_data)
					fname_test_data = fname_merge;
			}
			
			if (generate_train_test)
			{
				System.out.println("data repository: \n\t" + dir_rep);
				System.out.println("resample scheme: \n\t" + resample_scheme);
				System.out.println("class distribution in sample data: \n\t" + dist);
				System.out.println("train percentage: \n\t" + train_percent);
			}
			System.out.println("files train data: \n\t" + fname_train_data);
			System.out.println("files test data: \n\t" + fname_test_data);
			System.out.println("file prediction: \n\t" + fname_predict);
			System.out.println("log file (train): \n\t" + fname_log_train);
			System.out.println("log file (test): \n\t" + fname_log_test);
			System.out.println("classifiers: \n\t" + names.toString());
			System.out.println();

			// build classifier models
			ArrayList<String> fname_models = new ArrayList<String>();
			for (int idx = 0; idx < names.size(); idx++)
				fname_models.add(dir_experiment + fname_models_prefix 
									+ names.get(idx));
			System.out.println("files weka model: \n\t"  
									+ fname_models.toString());
			System.out.println();
			
			Main ctcmain = new Main();
			ctcmain.buildClassifier(names.toArray(new String[1]), 
									fname_train_data,
									fname_models.toArray(new String[1]),
									dir_experiment + fname_log_train);

			// evaluate models using test data
			ctcmain.evaluateData(fname_models.toArray(new String[1]),
								 fname_test_data,
								 dir_experiment + fname_predict,
								 dir_experiment + fname_log_test);
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
