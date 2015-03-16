=============================================================================
              CALTECH LARGE SCALE IMAGE SEARCH TOOLBOX  
=============================================================================

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 DESCRIPTION
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This C++/Matlab package implements several algorithms used for large scale
image search. The algorithms are implemented in C++, with an eye on large
scale databases. It can handle millions of images and hundreds of millions
of local features. It has MEX interfaces for Matlab, but can also be used
(with possible future modifications) from Python and directly from C++. It
can also be used for approximate nearest neighbor search, especially using
the Kd-Trees or LSH implementations.

The algorithms can be divided into two broad categories, depending on the
approach taken for image search:

1. Bag of Words:
----------------
The images are represented by histograms of visual words.

It includes algorithms for computing dictionaries:
* K-Means.
* Approximate K-Means (AKM).
* Hierarchical K-Means (HKM).

It also includes algorithms for fast search:
* Inverted File Index.
* Inverted File Index with Extra Information (for example for implementing
  Hamming Embedding).
* Min-Hash.

2. Full Representation:
-----------------------
The images are represented by the individual features.

It includes algorithms for fast approximate nearest neighbor search:

* Kd-Trees (Kdt).
* Hierarchical K-Means (Hkm).
* Locality Senstivie Hashing (LSH), with several hash functions:
** Hamming hash function (bit sampling, approximates hamming distance) i.e.
    h = x[i]
** Cosine hash function (random hyperplanes through the origin, approximates
    dot product) i.e. h = sign(<x,r>)
** L1 hash function (approximates the L1 distance) i.e. h = floor((x[i]-b) / w)    
** L2 hash function (random hyperplanes with bias, approximates
    euclidean distance, similar to E2LSH) i.e. h = floor((<x,r> - b) / w)
** Spherical Simplex (approximates distances on the unit hypersphere)
** Spherical Orthoplex (approximates distances on the unit hypersphere)
** Spherical Hypercube (approximates distances on the unit hypersphere)
** Binary Gausian Kernels (approximates gaussian kernel)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 CHANGES
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Nov. 5, 2010: version 1.0.


+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 CONTENTS
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Approximate K-Means
--------------------
ccvAkmeansClean.m: clears an AKM dictionary from memory
ccvAkmeansCreate.m: creates an AKM dictionary
ccvAkmeansLookup.m: looks up words in an AKM dictionary

Bag of Words
------------
ccvBowGetDict.m: creates a dictionary of visual words. Supports AKM & HKM.
ccvBowGetWordsClean.m: clears a dictionary from memory.
ccvBowGetWordsInit.m: initializes a dictionary to lookup a sequence of images.
ccvBowGetWords.m: looks up words in a dictionary. The typical sequence is to
    call ccvBowGetWordsInit at the start, then call ccvBowGetWords in a loop
    for different images, and finally call ccvBowGetWordsClean to clear it 
    from memory.

Distance 
--------
ccvDistance.m: computes distances between pairs of point sets.

Hierarchical K-Means
--------------------
ccvHkmClean.m: clears an HKM structure from memory.
ccvHkmCreate.m: creats and HKM structure.
ccvHkmExport.m: exports an HKM structure to Matlab.
ccvHkmImport.m: imports and HKM structure form Matlab.
ccvHkmKnn.m: performs k-nearest neighbor on an HKM structure.
ccvHkmLeafIds.m: retrieves the leaf id for input points. used in HKM 
    dictionaries as the visual words.

Inverted File
--------------
ccvInvFileClean.m: clears an inverted file from memory
ccvInvFileCompStats.m: prepares the inverted file for search operations.
ccvInvFileInsert.m: inserts docs in the inverted file
ccvInvFileLoad.m: loads an inverted file from a file
ccvInvFileSave.m: saves an inverted file to a file
ccvInvFileSearch.m: searches through the inverted file

Extra Inverted File (Hamming Embedding)
----------------------------------------
ccvInvFileExtraClean.m: clears an inverted file from memory
ccvInvFileExtraCompStats.m: prepares the inverted file for search operations.
ccvInvFileExtraInsert.m: inserts docs in the inverted file
ccvInvFileExtraSearch.m: searches through the inverted file

Kd-Tree
--------
ccvKdtClean.m: clears a Kdt structure from memory
ccvKdtCreate.m: creates a Kdt
ccvKdtKnn.m: performs k-nearest neighbor on the kdt
ccvKdtPoints.m: returns the points that share the same leaves without computing
  distances

K-Nearest Neighbor
-------------------
ccvKnn.m: performs brute force k-NN

Locality Sensitive Hashing
---------------------------
ccvLshBucketId.m: returns the id of the bucket
ccvLshBucketPoints.m: returns the points in a given bucket
ccvLshClean.m: clears an LSH from memory
ccvLshCreate.m: creates an LSH
ccvLshFuncVal.m: returns the values of the hash functions
ccvLshInsert.m: inserts into the LSH
ccvLshKnn.m: performs k-NN
ccvLshLoad.m: loads from a file
ccvLshSave.m: saves to a file
ccvLshSearch.m: returns points in the same bucket without distance computations
ccvLshStats.m: returns stats

ccvNormalize.m: normalizes input points
ccvNorm.m: returns the norm of the input points

ccvRandSeed.m: sets/restores the random seed

COMPILE.m: compiles the mex files

DEMO.m: demo file

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 INSTALL
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
To install package, unzip the package somewhere:

cd ~
unzip caltech-image-search.zip 
cd ~/caltech-image-search

Then compile the MEX files with Matlab:

matlab&
>> COMPILE

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 DEMO
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

See the demo file DEMO.m for example usages.

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 CONTACT
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Mohamed Aly <malaa at vision d0t caltech d0t edu>

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 REFERENCE
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

[1] Mohamed Aly, Mario Munich, and Pietro Perona.
Indexing in Large Scale Image Collections: Scaling Properties and Benchmark.
IEEE Workshop on Applications of Computer Vision WACV, January 2011.