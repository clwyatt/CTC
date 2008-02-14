package ctc.classification;

import java.util.ArrayList;

import weka.core.FastVector;
import weka.core.Instance;
import weka.core.Instances;

public final class WEKAutil {
	/**
	 * Find the index of an instance in several instances
	 * @param insts
	 * @param inst
	 * @return
	 */
    public static int[] indexOf(Instances insts, Instance inst)
    {
    	ArrayList<Integer> indices = new ArrayList<Integer>();
    	for (int i = 0; i < insts.numInstances(); i++)
    	{
    		boolean found = true;
    		for (int j = 0; j < insts.numAttributes(); j++)
    		{
    			if (insts.instance(i).value(j) != inst.value(j))
    			{
    				found = false;
    				break;
    			}
    		}
    		if (found)
    			indices.add(new Integer(i));
    	}
    	
    	int idx[] = new int[indices.size()];
    	for (int k = 0; k < indices.size(); k++)
    		idx[k] = indices.get(k).intValue();

    	return idx;
    }
    
    /**
     * Get all class values in a dataset.
     * @param insts
     * @return
     */
    public static double[] getClassValues(Instances insts)
    {
    	ArrayList<Double> class_values = new ArrayList<Double>();
    	
    	for (int i = 0; i < insts.numInstances(); i++)
    	{
        	if (class_values.contains(
        			new Double(insts.instance(i).classValue())) == false)
        		class_values.add(new Double(insts.instance(i).classValue()));
    	}

    	double retval[] = new double[class_values.size()];
    	for (int i = 0; i < class_values.size(); i++)
    		retval[i] = class_values.get(i);
    	
    	return retval;
    }
    
    /**
     * Find those instances whose class value equal to <param>class_value.
     * 
     * @param insts
     * @param class_value
     * @return
     */
    public static Instances instances(Instances insts, double class_value)
    {
    	Instances new_insts = new Instances(insts, 0);
    	for (int i = 0; i < insts.numInstances(); i++)
    	{
    		if (insts.instance(i).classValue() == class_value)
    			new_insts.add(insts.instance(i));
    	}
    	return new_insts;
    }
    
    public static Instances add(Instances insts_1, Instances insts_2)
    {
    	for (int i = 0; i < insts_2.numInstances(); i++)
    		insts_1.add(insts_2.instance(i));
    	return insts_1;
    }
    
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
