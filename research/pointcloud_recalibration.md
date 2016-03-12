##Point Cloud Transformation

Point clouds are a valuable way of getting depth readings from the drone. In the case that the point cloud is not what we expect, we will need to calculate a transformation to recalibrate at that point in time. We could make RFID "checkpoints" such that when we encounter a particular checkpoint RFID, then we can recalibrate using sonar. Regardless, the calculation would involve matrix multiplications and thus would not be computationally intensive enough to produce significant latency. The point cloud library (PCL) contains some good information on point cloud transformations and manipulations. Since we know our current point cloud and we know where we should be, we can find the optimal/best rotation and translation between two sets of corresponding 3D data. We need at least 3 points to solve this problem using a euclidean/rigid transform. We now present the solution overview.

Say that **R** is the rotation and **t** is the translation -- transforms that would be applied to dataset A to align it with dataset B. We are solving for **R** and **t** in the equation B = **R**A + **t**.

**Step 1**: Find the centroids of both datasets. The centroids are just the average points and can be calculated easily in constant time.

**Step 2**: Bring both dataset A and dataset B to the origin and then find the optimal rotation **R**. SVD (Singular Value Decomposition) is probably the easiest way of finding optimal rotations between points. Say the original matrix is **E**, then SVD will decompose/factorize a matrix into 3 other matrices such that **E = U S V_transpose**. Then, consider **H**, the familiar covariance matrix defined as the sum from i=1 to N of (P_A^i - centroid_A)(P_B^i - centroid_B)_transpose. Then, rotation can be found as **R = V U_transpose**.

**Step 3**: Find the translation **t**. In the rotation step, since we move both vectors to the origin and ignore the translation component, we now have to consider it. We get **t = -R centroidA + centroidB**.

The following paper describes the algorithm in more detail: http://www.math.zju.edu.cn/cagd/Seminar/2007_AutumnWinter/2007_Autumn_Master_LiuYu1_ref_1.pdf.
