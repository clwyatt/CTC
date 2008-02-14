package ctc.classification;

import org.junit.*;

import static org.junit.Assert.*;

import ctc.classification.CTCSpreadOversampleFilter;
import weka.core.Instances;
import weka.core.converters.ConverterUtils.DataSource;

public class CTCSpreadOversampleFilterTest 
{
    private CTCSpreadOversampleFilter filter;
    private Instances insts;

    /**
     * Sets up the test fixture. 
     * (Called before every test case method.)
     *
     */ 
    @Before 
    public void setUp() throws Exception
    {
        filter = new CTCSpreadOversampleFilter();
		DataSource ds_test = new DataSource("/home/haxu/program/weka-3-5-6/data/weather.arff");
		insts = ds_test.getDataSet();
		insts.setClassIndex(insts.numAttributes() - 1);
		filter.setInputFormat(insts);
    }

    /**
     * Tears down the test fixture. 
     * (Called after every test case method.)
     *
     */
    @After
    public void tearDown() throws Exception
    {
        filter = null;
    }

    @Test public void test0() throws Exception
    {
		assertTrue(insts.numInstances() == 14);
    }

    @Test public void test1()  throws Exception
    {
        filter.setDistributionSpread(0.0);
		Instances insts_filter_1 = weka.filters.Filter.useFilter(insts, filter);
		assertTrue(insts_filter_1.numInstances() == 14);
		assertTrue(insts_filter_1.instance(0).classValue() == 0);
		assertTrue(insts_filter_1.instance(1).classValue() == 0);
		assertTrue(insts_filter_1.instance(2).classValue() == 0);
		assertTrue(insts_filter_1.instance(3).classValue() == 0);
		assertTrue(insts_filter_1.instance(4).classValue() == 0);
		assertTrue(insts_filter_1.instance(5).classValue() == 0);
		assertTrue(insts_filter_1.instance(6).classValue() == 0);
		assertTrue(insts_filter_1.instance(7).classValue() == 0);
		assertTrue(insts_filter_1.instance(8).classValue() == 0);
		assertTrue(insts_filter_1.instance(9).classValue() == 1);
		assertTrue(insts_filter_1.instance(10).classValue() == 1);
		assertTrue(insts_filter_1.instance(11).classValue() == 1);
		assertTrue(insts_filter_1.instance(12).classValue() == 1);
		assertTrue(insts_filter_1.instance(13).classValue() == 1);
    }

    @Test public void test2() throws Exception
    {
		filter.setDistributionSpread(0.5);
		Instances insts_filter_2 = weka.filters.Filter.useFilter(insts, filter);
		assertTrue(insts_filter_2.numInstances() == 14);
		assertTrue(insts_filter_2.instance(0).classValue() == 0);
		assertTrue(insts_filter_2.instance(1).classValue() == 0);
		assertTrue(insts_filter_2.instance(2).classValue() == 0);
		assertTrue(insts_filter_2.instance(3).classValue() == 0);
		assertTrue(insts_filter_2.instance(4).classValue() == 0);
		assertTrue(insts_filter_2.instance(5).classValue() == 0);
		assertTrue(insts_filter_2.instance(6).classValue() == 0);
		assertTrue(insts_filter_2.instance(7).classValue() == 0);
		assertTrue(insts_filter_2.instance(8).classValue() == 0);
		assertTrue(insts_filter_2.instance(9).classValue() == 1);
		assertTrue(insts_filter_2.instance(10).classValue() == 1);
		assertTrue(insts_filter_2.instance(11).classValue() == 1);
		assertTrue(insts_filter_2.instance(12).classValue() == 1);
		assertTrue(insts_filter_2.instance(13).classValue() == 1);
    }
		
    @Test public void test3() throws Exception
    {
		filter.setDistributionSpread(1);
		Instances insts_filter_3 = weka.filters.Filter.useFilter(insts, filter);
		assertTrue(insts_filter_3.numInstances() == 18);
		assertTrue(insts_filter_3.instance(0).classValue() == 0);
		assertTrue(insts_filter_3.instance(1).classValue() == 0);
		assertTrue(insts_filter_3.instance(2).classValue() == 0);
		assertTrue(insts_filter_3.instance(3).classValue() == 0);
		assertTrue(insts_filter_3.instance(4).classValue() == 0);
		assertTrue(insts_filter_3.instance(5).classValue() == 0);
		assertTrue(insts_filter_3.instance(6).classValue() == 0);
		assertTrue(insts_filter_3.instance(7).classValue() == 0);
		assertTrue(insts_filter_3.instance(8).classValue() == 0);
		assertTrue(insts_filter_3.instance(9).classValue() == 1);
		assertTrue(insts_filter_3.instance(10).classValue() == 1);
		assertTrue(insts_filter_3.instance(11).classValue() == 1);
		assertTrue(insts_filter_3.instance(12).classValue() == 1);
		assertTrue(insts_filter_3.instance(13).classValue() == 1);
		assertTrue(insts_filter_3.instance(14).classValue() == 1);				
		assertTrue(insts_filter_3.instance(15).classValue() == 1);				
		assertTrue(insts_filter_3.instance(16).classValue() == 1);				
		assertTrue(insts_filter_3.instance(17).classValue() == 1);				
		// these instances are oversampled
		assertTrue(WEKAutil.indexOf(insts_filter_3, insts_filter_3.instance(14)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_3, insts_filter_3.instance(15)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_3, insts_filter_3.instance(16)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_3, insts_filter_3.instance(17)).length >= 2);
    }

    @Test public void test4() throws Exception
    {
		filter.setDistributionSpread(0);
		filter.setMinCount(6);
		Instances insts_filter_4 = weka.filters.Filter.useFilter(insts, filter);
		assertTrue(insts_filter_4.numInstances() == 15);
		assertTrue(insts_filter_4.instance(0).classValue() == 0);
		assertTrue(insts_filter_4.instance(1).classValue() == 0);
		assertTrue(insts_filter_4.instance(2).classValue() == 0);
		assertTrue(insts_filter_4.instance(3).classValue() == 0);
		assertTrue(insts_filter_4.instance(4).classValue() == 0);
		assertTrue(insts_filter_4.instance(5).classValue() == 0);
		assertTrue(insts_filter_4.instance(6).classValue() == 0);
		assertTrue(insts_filter_4.instance(7).classValue() == 0);
		assertTrue(insts_filter_4.instance(8).classValue() == 0);
		assertTrue(insts_filter_4.instance(9).classValue() == 1);
		assertTrue(insts_filter_4.instance(10).classValue() == 1);
		assertTrue(insts_filter_4.instance(11).classValue() == 1);
		assertTrue(insts_filter_4.instance(12).classValue() == 1);
		assertTrue(insts_filter_4.instance(13).classValue() == 1);
		assertTrue(insts_filter_4.instance(14).classValue() == 1);				
		// these instances are oversampled
		assertTrue(WEKAutil.indexOf(insts_filter_4, insts_filter_4.instance(14)).length >= 2);
    }	

    @Test public void test5() throws Exception
    {
		filter.setDistributionSpread(0.7);
		filter.setMinCount(7);
		Instances insts_filter_5 = weka.filters.Filter.useFilter(insts, filter);
		assertTrue(insts_filter_5.numInstances() == 16);
		assertTrue(insts_filter_5.instance(0).classValue() == 0);
		assertTrue(insts_filter_5.instance(1).classValue() == 0);
		assertTrue(insts_filter_5.instance(2).classValue() == 0);
		assertTrue(insts_filter_5.instance(3).classValue() == 0);
		assertTrue(insts_filter_5.instance(4).classValue() == 0);
		assertTrue(insts_filter_5.instance(5).classValue() == 0);
		assertTrue(insts_filter_5.instance(6).classValue() == 0);
		assertTrue(insts_filter_5.instance(7).classValue() == 0);
		assertTrue(insts_filter_5.instance(8).classValue() == 0);
		assertTrue(insts_filter_5.instance(9).classValue() == 1);
		assertTrue(insts_filter_5.instance(10).classValue() == 1);
		assertTrue(insts_filter_5.instance(11).classValue() == 1);
		assertTrue(insts_filter_5.instance(12).classValue() == 1);
		assertTrue(insts_filter_5.instance(13).classValue() == 1);
		assertTrue(insts_filter_5.instance(14).classValue() == 1);				
		assertTrue(insts_filter_5.instance(15).classValue() == 1);				
		// these instances are oversampled
		assertTrue(WEKAutil.indexOf(insts_filter_5, insts_filter_5.instance(14)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_5, insts_filter_5.instance(15)).length >= 2);
	}

    @Test public void test6() throws Exception
    {
		filter.setDistributionSpread(0);
		filter.setMinCount(10);
		Instances insts_filter_6 = weka.filters.Filter.useFilter(insts, filter);
		assertTrue(insts_filter_6.numInstances() == 20);
		assertTrue(insts_filter_6.instance(0).classValue() == 0);
		assertTrue(insts_filter_6.instance(1).classValue() == 0);
		assertTrue(insts_filter_6.instance(2).classValue() == 0);
		assertTrue(insts_filter_6.instance(3).classValue() == 0);
		assertTrue(insts_filter_6.instance(4).classValue() == 0);
		assertTrue(insts_filter_6.instance(5).classValue() == 0);
		assertTrue(insts_filter_6.instance(6).classValue() == 0);
		assertTrue(insts_filter_6.instance(7).classValue() == 0);
		assertTrue(insts_filter_6.instance(8).classValue() == 0);
		assertTrue(insts_filter_6.instance(9).classValue() == 0);
		assertTrue(insts_filter_6.instance(10).classValue() == 1);
		assertTrue(insts_filter_6.instance(11).classValue() == 1);
		assertTrue(insts_filter_6.instance(12).classValue() == 1);
		assertTrue(insts_filter_6.instance(13).classValue() == 1);
		assertTrue(insts_filter_6.instance(14).classValue() == 1);				
		assertTrue(insts_filter_6.instance(15).classValue() == 1);				
		assertTrue(insts_filter_6.instance(16).classValue() == 1);				
		assertTrue(insts_filter_6.instance(17).classValue() == 1);				
		assertTrue(insts_filter_6.instance(18).classValue() == 1);				
		assertTrue(insts_filter_6.instance(19).classValue() == 1);				
		// these instances are oversampled
		assertTrue(WEKAutil.indexOf(insts_filter_6, insts_filter_6.instance(9)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_6, insts_filter_6.instance(15)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_6, insts_filter_6.instance(16)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_6, insts_filter_6.instance(17)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_6, insts_filter_6.instance(18)).length >= 2);
		assertTrue(WEKAutil.indexOf(insts_filter_6, insts_filter_6.instance(19)).length >= 2);
    }
    
    @Test public void test7() throws Exception
    {
		filter.setDistributionSpread(2.0);
		Instances insts_filter = weka.filters.Filter.useFilter(insts, filter);
		assertTrue(insts_filter.numInstances() == 27);
		double[] class_values = WEKAutil.getClassValues(insts_filter);
		assertTrue(class_values.length == 2);
		assertTrue(WEKAutil.instances(insts_filter, class_values[0])
									 .numInstances() == 9);
		assertTrue(WEKAutil.instances(insts_filter, class_values[1])
									 .numInstances() == 18);
		// these instances are synthesized, should be unique
		for (int i = 15; i < insts_filter.numInstances(); i++)
			assertTrue(WEKAutil.indexOf(insts_filter, insts_filter.instance(i))
													.length >= 2);
    }
    
    /*
    @Test(expected=IndexOutOfBoundsException.class)
    public void testForException()
    {
        Object o = emptyList.get(0);
    }
    */
}
