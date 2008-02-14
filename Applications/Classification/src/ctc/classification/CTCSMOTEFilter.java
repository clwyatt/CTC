/**
 * 
 */
package ctc.classification;

import weka.filters.Filter;
import weka.filters.SupervisedFilter;
import weka.core.Instance;
import weka.core.Instances;
import weka.core.neighboursearch.LinearNNSearch;
import weka.core.Capabilities;
import weka.core.Capabilities.Capability;

import java.util.Random;
import java.util.ArrayList;

/**
 * Imlementation SMOTE resample method. 
 *
 * Reference:
 * N. V. Chawla, K. W. Bowyer, L. O. Hall, and W. P. Kegelmeyer, "Smote: Synthetic minority over-sampling technique," Journal of Artificial Intelligence Research, vol. 16, pp. 321-347, 2002.
 */
public class CTCSMOTEFilter extends Filter 
	implements SupervisedFilter
{
    private static final long serialVersionUID = -3947033795243930016L; // this is a copy from CTCSpreadOversampleFilter

	private int m_RandomSeed = 1;
	private double m_DistributionSpread = 0;
    private int m_MinCount;
    private int m_knn = 5;

    public void setDistributionSpread(double spread)
    { 
        m_DistributionSpread = spread;
    }

    public double getDistributionSpread() 
    { 
        return m_DistributionSpread;
    }

    public void setMinCount(int min)
    {
        m_MinCount = min;
    }

    public int getMinCount()
    {
        return m_MinCount;
    }

    public void setRandomSeed(int newSeed) 
    { 
        m_RandomSeed = newSeed;
    }

    public int getRandomSeed() 
    { 
        return m_RandomSeed;
    }

    public void setKNN(int knn)
    {
        m_knn = knn;
    }

    public int getKNN()
    {
        return m_knn;
    }

    /** 
    * Returns the Capabilities of this filter.
    *
    * @return            the capabilities of this object
    * @see               Capabilities
    */
    public Capabilities getCapabilities() 
    {
        Capabilities result = super.getCapabilities();

        // attributes
        result.enableAllAttributes();
        result.enable(Capability.MISSING_VALUES);

        // class
        result.enable(Capability.NOMINAL_CLASS);

        return result;
    }

    /**
    * Sets the format of the input instances.
    *
    * @param instanceInfo an Instances object containing the input 
    * instance structure (any instances contained in the object are 
    * ignored - only the structure is required).
    * @return true if the outputFormat may be collected immediately
    * @throws UnassignedClassException if no class attribute has been set.
    * @throws UnsupportedClassTypeException if the class attribute
    * is not nominal. 
    */
    public boolean setInputFormat(Instances instanceInfo) throws Exception 
    {
        super.setInputFormat(instanceInfo);
        setOutputFormat(instanceInfo);
        return true;
    }

    /**
    * Input an instance for filtering. Filter requires all training instances 
    * be read before producing output.
    *
    * @param instance the input instance
    * @return true if the filtered instance may now be collected with output().
    * @throws IllegalStateException if no input structure has been defined 
    */
    public boolean input(Instance instance) 
    { 
        if (getInputFormat() == null) 
            throw new IllegalStateException("No input instance format defined");
        
        if (m_NewBatch) 
        {
            resetQueue();
            m_NewBatch = false;
        }

        if (isFirstBatchDone()) 
        {
            push(instance);
            return true;
        } 
        else 
        {
            bufferInput(instance);
            return false;
        }
    }

    /**
    * Signify that this batch of input to the filter is finished. 
    * If the filter requires all instances prior to filtering,
    * output() may now be called to retrieve the filtered instances.
    *
    * @return true if there are instances pending output
    * @throws IllegalStateException if no input structure has been defined
    */
    public boolean batchFinished() throws Exception
    { 
        if (getInputFormat() == null)
            throw new IllegalStateException("No input instance format defined");

        if (!isFirstBatchDone()) 
            _createOverSample();

        flushInput();
        m_NewBatch = true;
        m_FirstBatchDone = true;

        return (numPendingOutput() != 0);
    }

    protected void _createOverSample() throws Exception
    {
        int classI = getInputFormat().classIndex();
        // Sort according to class attribute.
        getInputFormat().sort(classI);
        // Determine where each class starts in the sorted dataset
        int [] classIndices = _getClassIndices();

        // Get the existing class distribution
        int [] counts = new int [getInputFormat().numClasses()];
        double [] weights = new double [getInputFormat().numClasses()];
        for (int i = 0; i < getInputFormat().numInstances(); i++) 
        {
            Instance current = getInputFormat().instance(i);
            if (current.classIsMissing() == false) 
            {
                counts[(int)current.classValue()]++;
                weights[(int)current.classValue()]+= current.weight();
            }
        }

        // find the class with the maximum number of instances
        int max = counts[0];
        for (int i = 1; i < counts.length; i++) 
            if (counts[i] > max)
                max = counts[i];
        if (max <= 0)
            throw new Exception("majority class has # of instances: " + max);

        // determine the new distribution 
        int [] new_counts = new int [getInputFormat().numClasses()];
        for (int i = 0; i < counts.length; i++) 
        {
            if (m_DistributionSpread == 0) 
            {
                new_counts[i] = counts[i];
            }
            else 
            {
            	// the number of majority class or classes does not change
	        	if (max == counts[i])
	        		new_counts[i] = counts[i];
	        	else
	        		new_counts[i] = (int)Math.abs(Math.max(counts[i],
	        									  max * m_DistributionSpread));
            }
            
            if (m_MinCount > 0) 
                new_counts[i] = Math.max(new_counts[i], m_MinCount);
        }

        // TODO
        //   evaluate other NearestNeighbourSearch algorithms
        LinearNNSearch alg_knn = new LinearNNSearch();
        Random random = new Random(m_RandomSeed);
        for (int j = 0; j < new_counts.length; j++) 
        { 
            // get instances within a class
            Instances insts_within_class = new Instances(getInputFormat(), classIndices[j], counts[j]);
            alg_knn.setInstances(insts_within_class);

            // add those original samples from each class
            for (int k = 0; k < counts[j]; k++)
            {
                int index = classIndices[j] + k;
                Instance newInst = (Instance)getInputFormat().instance(index).copy();
                push(newInst);
            }

            // synthesize over-sampling 
            for (int k = 0; k < new_counts[j] - counts[j]; k++) {
                int index = classIndices[j] + (Math.abs(random.nextInt()) 
                         % (classIndices[j + 1] - classIndices[j])) ;
                Instance inst = (Instance)getInputFormat().instance(index).copy();
                Instances insts_knn = alg_knn.kNearestNeighbours(inst, m_knn + 1);
                int tmp[] = WEKAutil.indexOf(insts_knn, inst);
                if (tmp.length == 1)
                	insts_knn.delete(tmp[0]);
                else
                	throw new Exception("WEKA fixed the bug in LinearNNSearch::kNearestNeighbours, which returns the centerpoint itself. (5NN will return 4NN and the centerpoint itself.");
                
                // random select one of KNNs to be used to synthesize new inst
                int idx_random = Math.abs(random.nextInt()) % m_knn;
                Instance inst_1nn = insts_knn.instance(idx_random);
                inst = _synthesizeInst(inst, inst_1nn);
                push(inst);
            }
        }
    }

    /**
    * Creates an index containing the position where each class starts in 
    * the getInputFormat(). m_InputFormat must be sorted on the class attribute.
    * 
    * @return the positions
    */
    protected int[] _getClassIndices() 
    { 
        // Create an index of where each class value starts
        int [] classIndices = new int [getInputFormat().numClasses() + 1];
        int currentClass = 0;
        classIndices[currentClass] = 0;
        for (int i = 0; i < getInputFormat().numInstances(); i++) 
        {
            Instance current = getInputFormat().instance(i);
            if (current.classIsMissing()) 
            {
                for (int j = currentClass + 1; j < classIndices.length; j++) 
                    classIndices[j] = i;
                break;
            } 
            else if (current.classValue() != currentClass) 
            {
                for (int j = currentClass + 1; j <= current.classValue(); j++)
                    classIndices[j] = i;
                currentClass = (int) current.classValue();
            }
        }
        if (currentClass <= getInputFormat().numClasses()) 
            for (int j = currentClass + 1; j < classIndices.length; j++) 
                classIndices[j] = getInputFormat().numInstances();

        return classIndices;
    }

    /**
     * Synthesize a new instance. Assume all attributes are numerical (no nominal attributes).
     * @param  inst  original instance.
     * @param  inst_1nn  a random choose instance from kNNs.
     * @return synthetic instance.
     */
    protected Instance _synthesizeInst(Instance inst, Instance inst_1nn)
    {
        double gap = Math.random();
        Instance new_inst = new Instance(inst);
        
        for (int i = 0; i < inst.numAttributes(); i++)
            new_inst.setValue(i, inst.value(i) + gap * (inst_1nn.value(i) - inst.value(i)));

        return new_inst;
    }

	public static void main(String args[])
	{
	}

}

