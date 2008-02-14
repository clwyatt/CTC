package ctc.classification;

import weka.classifiers.Classifier;
import weka.classifiers.Evaluation;
import weka.core.Instances;

public class CTCEvaluation extends Evaluation{
	protected Instances m_insts;
	protected Classifier m_classifier;
	protected double[] m_predicts;  // store the class index

	public CTCEvaluation(Instances data) throws Exception
	{
		super(data);
	}
	
	public void evaluate(Classifier clsifier, Instances insts) throws Exception
	{
		m_classifier = clsifier;
		m_insts = insts;
		m_predicts = evaluateModel(clsifier, insts);
	}
	
	public String[] getPredicts()
	{
		String[] preds = new String[m_predicts.length];
		
		for (int idx = 0; idx < m_predicts.length; idx++)
			preds[idx] = m_ClassNames[(int)m_predicts[idx]];
		
		return preds;
	}
	
	public String exportXML()
	{
		return "";
	}
	
	public String exportPredictXML()
	{
		return "";
	}

	public String exportPredict()
	{
		/*
		String truelabel;
		String predict;
		String endl = System.getProperty("line.separator");
		String csv = new String();
		
		for (int idx = 0; idx <= m_insts.numInstances(); idx++)
		{
			truelabel = m_insts.attribute(m_insts.classIndex()).value(idx);
			predict = Double.toString(m_predicts[idx]);
			csv += truelabel + "," + predict + endl;
		}
		*/
		return "";
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
