package ctc.classification;

import java.util.Collections;
import java.util.Random;
import java.util.Vector;

import weka.core.Instance;
import weka.core.Instances;
import weka.core.Queue;
import weka.core.converters.ConverterUtils.DataSource;
import weka.filters.supervised.instance.Resample;

public class CTCResampleFilter extends Resample {
	private Instances m_remainsFormat;
	private Queue m_remainsQueue;
	
	protected void push_remains(Instance instance) 
	{
		if (instance != null) 
		{
			if (instance.dataset() != null)
				copyValues(instance, false);
	      instance.setDataset(m_remainsFormat);
	      m_remainsQueue.push(instance);
	    }
	}
	
	public Instances getRemains()
	{
	    Instances newData = new Instances(m_remainsFormat, 0);

	    if (m_remainsFormat == null) {
	        throw new NullPointerException("No output instance format defined");
	      }
	      if (m_remainsQueue.empty()) {
	        return null;
	      }
	
	      while (!m_remainsQueue.empty())
	    	  newData.add((Instance)m_remainsQueue.pop());
	      
	      return newData;
	}


	  /**
	   * Modify to keep remaining instances.
	   * Fix the randomness bug in Resample filter in weka. 
	   * TODO: confirm this is a bug or not.
	   * 
 	   * in this method of weka's Resample, there are two calls on random. And 
	   * this violate the randomness in its generating integers. 
	   *   random.nextDouble();
	   *   random.nextInt(); 
	   * 
	   * 
	   * @param random		the random number generator to use
	   * @param origSize		the original size of the dataset
	   * @param sampleSize		the size to generate
	   * @param actualClasses	the number of classes found in the data
	   * @param classIndices	the indices where classes start
	   */
	  public void createSubsampleWithoutReplacement(Random random, int origSize, 
	      int sampleSize, int actualClasses, int[] classIndices) 
	  {
		    if (sampleSize > origSize) {
		        sampleSize = origSize;
		        System.err.println(
		  	  "Resampling with replacement can only use percentage <=100% - "
		  	  + "Using full dataset!");
		      }

		      Vector<Integer>[] indices = new Vector[actualClasses];
		      Vector<Integer>[] indicesNew = new Vector[actualClasses];

		      // generate list of all indices to draw from
		      for (int i = 0; i < actualClasses; i++) {
		        indices[i] = new Vector<Integer>(classIndices[i + 1] - classIndices[i]);
		        indicesNew[i] = new Vector<Integer>(indices[i].capacity());
		        for (int n = classIndices[i]; n < classIndices[i + 1]; n++)
		  	indices[i].add(n);
		      }

		      // draw X samples
		      int currentSize = origSize;
		      for (int i = 0; i < sampleSize; i++) {
		        int index = 0;
		        if (random.nextDouble() < m_BiasToUniformClass) {
		  	// Pick a random class (of those classes that actually appear)
		  	int cIndex = random.nextInt(actualClasses);
		  	for (int j = 0, k = 0; j < classIndices.length - 1; j++) {
		  	  if ((classIndices[j] != classIndices[j + 1]) 
		  	      && (k++ >= cIndex)) {
		  	    // Pick a random instance of the designated class
		  	    index = random.nextInt(indices[j].size());
		  	    indicesNew[j].add(indices[j].get(index));
		  	    indices[j].remove(index);
		  	    break;
		  	  }
		  	}
		        }
		        else {
		  	index = random.nextInt(currentSize);
		  	for (int n = 0; n < actualClasses; n++) {
		  	  if (index < indices[n].size()) {
		  	    indicesNew[n].add(indices[n].get(index));
		  	    indices[n].remove(index);
		  	    break;
		  	  }
		  	  else {
		  	    index -= indices[n].size();
		  	  }
		  	}
		  	currentSize--;
		        }
		      }

		      // sort indices
		      if (getInvertSelection()) {
		        indicesNew = indices;
		      }
		      else {
		        for (int i = 0; i < indicesNew.length; i++)
		  	Collections.sort(indicesNew[i]);
		      }

		      // add to ouput
		      for (int i = 0; i < indicesNew.length; i++) {
		        for (int n = 0; n < indicesNew[i].size(); n++)
		  	push((Instance) getInputFormat().instance(indicesNew[i].get(n)).copy());
		      }

		    // add to remains
		    m_remainsFormat = getOutputFormat();
		    m_remainsQueue = new Queue();
		    for (int i = 0; i < indices.length; i++)
		    {
		    	for (int n = 0; n < indices[i].size(); n++)
		    		push_remains((Instance) getInputFormat().instance(indices[i].get(n)).copy());
		    }

			  // clean up
		      for (int i = 0; i < indices.length; i++) {
		        indices[i].clear();
		        indicesNew[i].clear();
		      }
		      indices = null;
		      indicesNew = null;	  
	  }
	  
	public static void main(String args[])
	{
		try{				
			CTCResampleFilter filter = new CTCResampleFilter();
			filter.test_getRemains();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	protected void test_getRemains() throws Exception
	{
		DataSource ds_test = new DataSource("/home/haxu/program/weka-3-5-6/data/weather.arff");
		Instances insts = ds_test.getDataSet();
		insts.setClassIndex(insts.numAttributes() - 1);
		this.setSampleSizePercent(80);
		this.setNoReplacement(true);
		this.setInputFormat(insts);
		Instances inst_train = weka.filters.Filter.useFilter(insts, this);
		Instances inst_test = this.getRemains();
		assert(inst_train.numInstances() + inst_test.numInstances() == insts.numInstances());
	}
}


