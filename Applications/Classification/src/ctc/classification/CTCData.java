package ctc.classification;

import weka.core.Instances;
import weka.core.converters.ConverterUtils.DataSource;
import weka.core.converters.ArffSaver;
import weka.filters.Filter;
import weka.filters.supervised.instance.SpreadSubsample;

import java.io.File;
import java.io.FileWriter;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.util.ArrayList;

public class CTCData 
{
	protected ArrayList<String> m_fname_train = new ArrayList<String>();
	protected ArrayList<String> m_fname_test = new ArrayList<String>();

    public enum RESAMPLE_SCHEME {
        OVERSAMPLE,
        UNDERSAMPLE,
        SMOTE
    }

    public int getNumTrainTestFile() throws Exception
    {
    	if (m_fname_train.size() != m_fname_test.size())
    		throw new Exception("System error: \n" + 
    				"m_fname_train.size() != m_fname_test.size()");
    	
    	return m_fname_train.size();
    }
    
	public String getTrainFilename() {
		return m_fname_train.get(0);
	}

	public String getTestFilename() {
		return m_fname_test.get(0);
	}

	public String getTrainFilename(int i) {
		return m_fname_train.get(i);
	}

	public String getTestFilename(int i) {
		return m_fname_test.get(i);
	}

	protected String _getArffFileHeader() 
	{
		String endl = System.getProperty("line.separator");
		String h = new String();
		h = "@relation 'ctc'" + endl + endl + "@attribute polypid string"
				+ endl + "@attribute x numeric" + endl + "@attribute y numeric"
				+ endl + "@attribute z numeric" + endl
				+ "@attribute f4 numeric" + endl + "@attribute f5 numeric"
				+ endl + "@attribute f6 numeric" + endl
				+ "@attribute f7 numeric" + endl + "@attribute f8 numeric"
				+ endl + "@attribute f9 numeric" + endl
				+ "@attribute f10 numeric" + endl + "@attribute f11 numeric"
				+ endl + "@attribute f12 numeric" + endl
				+ "@attribute f13 numeric" + endl + "@attribute f14 numeric"
				+ endl + "@attribute f15 numeric" + endl
				+ "@attribute f16 numeric" + endl + "@attribute f17 numeric"
				+ endl + "@attribute f18 numeric" + endl
				+ "@attribute class {0,1}" + endl + endl + "@data" + endl;

		return h;
	}

	/**
	 * Merge feature files for each CT volume into one ARFF file. Each feature 
	 * file is a CSV file stored in directory of each CT volume. 
	 * 
	 * @param rep_dir  Name of the directory hold all CT volumes. 
	 * @param output_dir  Name of the directory to store the ARFF file.
	 * @return Absolute filename for the ARFF file.
	 * @throws Exception
	 */
	public String mergeRepository(String rep_dir, String output_dir) 
					throws Exception
	{
		if (new File(rep_dir).isDirectory() == false)
			throw new IllegalArgumentException("Invalid repository: "
												+ rep_dir);
		if (new File(output_dir).isDirectory() == false)
			throw new IllegalArgumentException("Invalid output directory: "
												+ output_dir);
		if (rep_dir.charAt(rep_dir.length() - 1) != File.separatorChar)
			rep_dir += File.separator;
		if (output_dir.charAt(output_dir.length() - 1) != File.separatorChar)
			output_dir += File.separator;
		
		// list all feature files
		String C_FNAME_POLYP_FEATURE = "ExtractedPolyps.mat.matched.2.csv";

		ArrayList<String> fname_polyp_feature = new ArrayList<String>();

		File rep = new File(rep_dir);
		File[] rep_subdirs = rep.listFiles();
		for (int idx = 0; idx < rep_subdirs.length; idx++) 
		{
			if (rep_subdirs[idx].isDirectory() == false)
				continue;
			File[] rep_sub_subdirs = rep_subdirs[idx].listFiles();
			for (int idy = 0; idy < rep_sub_subdirs.length; idy++) 
			{
				if (rep_sub_subdirs[idy].isDirectory() == false)
					continue;

				if (new File(rep_sub_subdirs[idy] + File.separator
						+ C_FNAME_POLYP_FEATURE).exists()) 
				{
					fname_polyp_feature.add(rep_sub_subdirs[idy]
							+ File.separator + C_FNAME_POLYP_FEATURE);
				}
			}
		}

		// merge all feature files into one ARFF file
		String fname_merged = output_dir + C_FNAME_POLYP_FEATURE + ".arff";
		BufferedWriter f_arff = new BufferedWriter(
										new FileWriter(fname_merged));
		f_arff.write(_getArffFileHeader());
		for (int idx = 0; idx < fname_polyp_feature.size(); idx++) 
		{
			String polypid_prefix = new String(fname_polyp_feature.get(idx));
			polypid_prefix = polypid_prefix.
					substring(0, polypid_prefix.indexOf(C_FNAME_POLYP_FEATURE));
			polypid_prefix = polypid_prefix.substring(rep_dir.length());
			BufferedReader f_csv = new BufferedReader(new FileReader(
					fname_polyp_feature.get(idx)));
			for (String line = f_csv.readLine(); 
				 line != null;
				 line = f_csv.readLine())
			{
				f_arff.write(polypid_prefix + line + 
								System.getProperty("line.separator"));
			}
			f_csv.close();
		}
		f_arff.close();
		
		return fname_merged;
	}
	
	/**
	 * Resample data using over-sample, under-sample or SMOTE method.
	 * 
	 * @param fname_in  Feature file contains sample data. Assume the last 
	 * 					column hold the class information.
	 * @param output_dir  Name of directory storing the new feature file. 
	 * @param scheme  OVERSAMPLE, UNDERSAMPLE or SMOTE.
	 * @param dist  ratio of FP/TP for UNDERSAMPLE.
	 *              ratio of TP/FP for OVERSAMPLE and SMOTE.
	 *              (assume TP class is minority class).
	 * @return Name for the new feature file.
	 * @throws Exception
	 */
	public String resample(String fname_in,
						   String output_dir,
						   RESAMPLE_SCHEME scheme,
						   double dist) throws Exception 
	{
		if (new File(fname_in).exists() == false)
			throw new IllegalArgumentException("Invalid feature file: "
												+ fname_in);
		if (new File(output_dir).isDirectory() == false)
			throw new IllegalArgumentException("Invalid output directory: "
												+ output_dir);
		if (output_dir.charAt(output_dir.length() - 1) != File.separatorChar)
			output_dir += File.separator;
		
		// read data
		DataSource ds_test = new DataSource(fname_in);
		Instances insts = ds_test.getDataSet();
		insts.setClassIndex(insts.numAttributes() - 1);
		
		// re-sample
		String fname_out = new String();
		if (scheme == RESAMPLE_SCHEME.OVERSAMPLE)
		{
			CTCSpreadOversampleFilter filter1 = new CTCSpreadOversampleFilter();
			filter1.setDistributionSpread(dist);
			filter1.setInputFormat(insts);
			insts = Filter.useFilter(insts, filter1);
			fname_out = output_dir + ".SMOTE." + Double.toString(dist) +".arff";
		}
		else if (scheme == RESAMPLE_SCHEME.UNDERSAMPLE)
		{
			SpreadSubsample filter1 = new SpreadSubsample();
			filter1.setDistributionSpread(dist);
			filter1.setInputFormat(insts);
			if (dist < 1)
				throw new Exception("Bug in weka if class distribution is " + 
						"less than 1.0 when using UNDERSAMPLE method. Current" +
						" class distribution is: " + Double.toString(dist));
			insts = Filter.useFilter(insts, filter1);
			fname_out = output_dir + ".SMOTE." + Double.toString(dist) +".arff";
		}
		else if (scheme == RESAMPLE_SCHEME.SMOTE)
		{
			CTCSMOTEFilter filter1 = new CTCSMOTEFilter();
			filter1.setDistributionSpread(dist);
			filter1.setInputFormat(insts);
			insts = Filter.useFilter(insts, filter1);
			fname_out = output_dir + ".SMOTE." + Double.toString(dist) +".arff";
		}
		else
		{
			throw new Exception("non-implemention.");
		}
		
		// write new feature file in ARFF format.
		ArffSaver writer = new ArffSaver();
		writer.setInstances(insts);
		writer.setFile(new File(fname_out));
		writer.writeBatch();
		
		return fname_out;
	}
	
	/**
	 * Generate train/test file for a certain train percentage.
	 * 
	 * @param fname  Feature file contains sample data. Assume the last column 
	 *               hold the class information. 
	 * @param output_dir  Name of directory holds train/test files.  
	 * @param train_percent
	 * @throws Exception
	 */
	public void generateTrainTestFile(String fname,
									  String output_dir,
									  double train_percent) throws Exception
	{
		if (new File(fname).exists() == false)
			throw new IllegalArgumentException("Invalid feature file: "
												+ fname);
		if (new File(output_dir).isDirectory() == false)
			throw new IllegalArgumentException("Invalid output directory: "
												+ output_dir);
		if (train_percent <= 0 || train_percent >= 100)
			throw new IllegalArgumentException("Invalid train percent: " 
												+ train_percent);
		if (output_dir.charAt(output_dir.length() - 1) != File.separatorChar)
			output_dir += File.separator;

		// read data
		DataSource ds_test = new DataSource(fname);
		Instances insts = ds_test.getDataSet();
		insts.setClassIndex(insts.numAttributes() - 1);

		// split train/test
		CTCResampleFilter filter2 = new CTCResampleFilter();
		filter2.setSampleSizePercent(100 - train_percent);
		filter2.setBiasToUniformClass(0);
		filter2.setNoReplacement(true);
		filter2.setInputFormat(insts);
		Instances insts_test = Filter.useFilter(insts, filter2);
		Instances insts_train = filter2.getRemains();

		// write data
		String fname_train = output_dir + "train." + 
								Double.toString(train_percent) + ".arff";
		String fname_test = output_dir +  "test." + 
								Double.toString(100 - train_percent) + ".arff";
		m_fname_train.clear();
		m_fname_test.clear();
		m_fname_train.add(fname_train);
		m_fname_test.add(fname_test);

		ArffSaver writer = new ArffSaver();
		writer.setInstances(insts_test);
		writer.setFile(new File(fname_test));
		writer.writeBatch();
		writer.setInstances(insts_train);
		writer.setFile(new File(fname_train));
		writer.writeBatch();
	}
	
	/**
	 * Generate cross validation data.
	 * 
	 * @param fname
	 * @param output_dir
	 * @param num_folds
	 * @throws Exception
	 */
	public void generteTrainTestFile_CrossValidation(
					String fname, 
					String output_dir,
					int num_folds) throws Exception
	{
		if (new File(fname).exists() == false)
			throw new IllegalArgumentException("Invalid feature file: "
												+ fname);
		if (new File(output_dir).isDirectory() == false)
			throw new IllegalArgumentException("Invalid output directory: "
												+ output_dir);
		if (num_folds < 0)
			throw new IllegalArgumentException("Invalid number of folds: " 
												+ num_folds);
		if (output_dir.charAt(output_dir.length() - 1) != File.separatorChar)
			output_dir += File.separator;
		
		// read data
		DataSource ds_test = new DataSource(fname);
		Instances insts = ds_test.getDataSet();
		insts.setClassIndex(insts.numAttributes() - 1);
		
		Instances insts_train = new Instances(insts, 0);
		Instances insts_test = new Instances(insts, 0);
		ArffSaver writer = new ArffSaver();
		m_fname_train.clear();
		m_fname_test.clear();
		
		double class_values[] = WEKAutil.getClassValues(insts);
		for (int idx_fold = 0; idx_fold < num_folds; idx_fold++)
		{
			insts_train.delete();
			insts_test.delete();
			for (int i = 0; i < class_values.length; i++)
			{
				Instances insts_cls = WEKAutil.instances(insts, class_values[i]);
				insts_train = WEKAutil.add(insts_train,
									insts_cls.trainCV(num_folds, idx_fold));
				insts_test = WEKAutil.add(insts_test,
									insts_cls.testCV(num_folds, idx_fold));
			}
			String fname_train = 
				output_dir + "train.x-val." + Integer.toString(num_folds) 
					+ "-" + Integer.toString(idx_fold) + ".arff";
			String fname_test = 
				output_dir + "test.x-val." + Integer.toString(num_folds) 
					+ "-" + Integer.toString(idx_fold) + ".arff";

			m_fname_train.add(fname_train);
			m_fname_test.add(fname_test);
			
			// write train/test data
			writer.setInstances(insts_test);
			writer.setFile(new File(fname_test));
			writer.writeBatch();
			writer.setInstances(insts_train);
			writer.setFile(new File(fname_train));
			writer.writeBatch();
		}
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			
			CTCData ctcdata = new CTCData();
			//ctcdata.generateTrainTestFile_CrossValidation("/home/haxu/workspace/projects/CTC_Classification/Output/SMOTE.5.0/ExtractedPolyps.mat.matched.2.csv.arff", "./", 5);
			for (int i = 0; i < 5; i++)
			{
				String fname = "/home/haxu/workspace/projects/CTC_Classification/Output/REPTree/train.x-val.5-" + Integer.toString(i) + ".arff"; 
				ctcdata.resample(fname, "./", CTCData.RESAMPLE_SCHEME.UNDERSAMPLE, 0.5);
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
