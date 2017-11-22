# TeethClassifier (2017)
Image Classifier - Classify frontal faces into two classes: shows teeth and shows not teeth

Technologies:
 - Python 2.7
 - Tensor Flow
 - OpenCV
 - Scikit-neuralnetworks
 - Scikit-learn
 - Scikit-images
 - numpy
 - Convolutional Neural Networks
 - ImageNet

# How to Run First Prototype

<b>Training</b>

<code>python tf_trainer.py --bottleneck_dir=bottlenecks --how_many_training_steps=4000 --model_dir=inception --summaries_dir=training_summaries/basic --output_graph=retrained_graph.pb --output_labels=retrained_labels.txt --image_dir=teeth_images_org/ --learning_rate=0.005</code>

<b>Classification</b>

<code>python tf_classifier.py ./B ./results</code>
 
 # How to Run Second Prototype

<b>Training the CNN</b>

<code>python sknn_trainer.py ./all_teeth_org/ cnn_graph.nn ov 0</code>

<b>Classification</b>

<code>python sknn_classifier.py ./B ./results cnn_graph.nn</code>

<code>./B</code> is the directory with the images to classify<br>
<code>./results</code> is the directory where the classified images will be saved, one sub-directory for each class
