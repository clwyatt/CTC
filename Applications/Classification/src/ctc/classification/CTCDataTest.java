package ctc.classification;

import org.junit.*;
import static org.junit.Assert.*;
import java.io.File;
import weka.core.Instances;
import weka.core.converters.ConverterUtils.DataSource;

public class CTCDataTest 
{
	private static CTCData m_ctcdata;
	private static String m_fname_merged;
	
	@BeforeClass
	public static void oneTimeSetup() throws Exception
	{
		m_ctcdata = new CTCData();
		String rep_dir = "/gpfs0/santagoGrp/data/WFU_prior_CTC_data/";
		String output_dir = System.getProperty("java.io.tmpdir");
		m_fname_merged = m_ctcdata.mergeRepository(rep_dir, output_dir);
	}
	
	@AfterClass
	public static void oneTimeTearDown()
	{
		m_ctcdata = null;
	}
	
	@Before
	public void setUp()
	{
	}
	
	@After
	public void tearDown()
	{
	}
	
	@Test
	public void test_mergeRepository() throws Exception
	{
		assertTrue(new File(m_fname_merged).exists());
		
		// assert the last column is class index
		DataSource ds = new DataSource(m_fname_merged);
		Instances insts = ds.getDataSet();
		insts.setClassIndex(insts.numAttributes() - 1);
		double class_values[] = WEKAutil.getClassValues(insts);
		for (int i = 0; i < class_values.length; i++)
			assertTrue(class_values[i] == 0 || class_values[i] == 1);
	}
	
	@Test
	public void test_resample() throws Exception
	{
		String output_dir = System.getProperty("java.io.tmpdir");
		String fname_out;

		fname_out = m_ctcdata.resample(m_fname_merged,
									   output_dir,
									   CTCData.RESAMPLE_SCHEME.OVERSAMPLE,
									   0.1);
		assertTrue(new File(fname_out).exists());

		fname_out = m_ctcdata.resample(m_fname_merged,
									   output_dir,
									   CTCData.RESAMPLE_SCHEME.UNDERSAMPLE,
									   50);
		assertTrue(new File(fname_out).exists());

		fname_out = m_ctcdata.resample(m_fname_merged,
									   output_dir,
									   CTCData.RESAMPLE_SCHEME.SMOTE,
									   9);
		assertTrue(new File(fname_out).exists());
	}

	@Test
	public void test_generateTrainTestFile() throws Exception
	{
		double train_percent = 80;
		String output_dir = System.getProperty("java.io.tmpdir");
		m_ctcdata.generateTrainTestFile(m_fname_merged, 
										output_dir,
										train_percent);
		String fname_train = m_ctcdata.getTrainFilename();
		String fname_test = m_ctcdata.getTestFilename();
		assertTrue(m_ctcdata.getNumTrainTestFile() == 1);
		assertTrue(new File(fname_train).exists());
		assertTrue(new File(fname_test).exists());
		
		DataSource ds_train = new DataSource(fname_train);
		Instances insts_train = ds_train.getDataSet();
		DataSource ds_test = new DataSource(fname_test);
		Instances insts_test = ds_test.getDataSet();
		DataSource ds_orig = new DataSource(m_fname_merged);
		Instances insts_orig = ds_orig.getDataSet();
		
		// 
		double tol = 0.01;
		assertTrue(
			Math.abs((insts_orig.numInstances() * train_percent / 100 
					  - insts_train.numInstances())
					 / insts_orig.numInstances()) < tol);
		assertTrue(
				Math.abs((insts_orig.numInstances() * (1 - train_percent / 100) 
						  - insts_test.numInstances())
						 / insts_orig.numInstances()) < tol);
		Instances insts_merged = WEKAutil.add(insts_train, insts_test);
		assertTrue(insts_orig.numInstances() == insts_merged.numInstances());
	}
	
	@Test 
	public void test_generateTrainTestFile_CrossValidation() throws Exception
	{
		int num_folds = 10;
		String output_dir = System.getProperty("java.io.tmpdir");
		m_ctcdata.generteTrainTestFile_CrossValidation(m_fname_merged, 
													   output_dir,
													   num_folds);
		assertTrue(m_ctcdata.getNumTrainTestFile() == 10);

		DataSource ds_orig = new DataSource(m_fname_merged);
		Instances insts_orig = ds_orig.getDataSet();
		for (int i = 0; i < m_ctcdata.getNumTrainTestFile(); i++)
		{
			String fname_train = m_ctcdata.getTrainFilename(i);
			String fname_test = m_ctcdata.getTestFilename(i);
			assertTrue(new File(fname_train).exists());
			assertTrue(new File(fname_test).exists());

			DataSource ds_train = new DataSource(fname_train);
			Instances insts_train = ds_train.getDataSet();
			DataSource ds_test = new DataSource(fname_test);
			Instances insts_test = ds_test.getDataSet();
			
			// 
			assertTrue(insts_train.numInstances() - 
					   insts_test.numInstances() * (num_folds - 1)
						<= insts_test.numInstances());
			Instances insts_merged = WEKAutil.add(insts_train, insts_test);
			assertTrue(insts_orig.numInstances() == insts_merged.numInstances());		
		}
	}
}
