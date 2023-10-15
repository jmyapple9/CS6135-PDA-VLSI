# Homework 2: Grading Script
We will use this script to judge your program.  
**Please make sure your program can be executed by this script.**

## Preparing
* Step1:  
    Go into directory `student` and generate a new directory with your student id.
    ```shell
    $ cd student/
    $ mkdir ${your_student_id}
    $ cd ${your_student_id}/
    ```

    e.g.:
    ```shell
    $ cd student/
    $ mkdir 112062500
    $ cd 112062500/
    ```

* Step2:  
    Put your compressed file in the directory which you just generated.  
    The whole path is as follow:
    ```
    HW2_grading/student/${your_student_id}/CS6135_HW2_${your_student_id}.tar.gz
    ```

    e.g.:
    ```
    HW2_grading/student/112062500/CS6135_HW2_112062500.tar.gz
    ```

### Notice:
**Do not put your original directory here** because it will remove all directories before unzipping the compressed file.

## Working Flow
* Step1:  
    Go into directory `HW2_grading` and change the permission of `HW2_grading.sh`.
    ```sh
    $ chmod 500 HW2_grading.sh
    ```

* Step2:  
    Run `HW2_grading.sh`.
    ```sh
    $ bash HW2_grading.sh
    ```

* Step3:  
    Check your output.
    * If the status is **success**, it means your program finished in time and your output is correct. E.g.,
        ```
        grading on 112062500:
          testcase |    cutsize |    runtime | status
           public1 |        251 |       0.04 | success
           public2 |       1657 |      22.61 | success
           public3 |      11661 |     254.15 | success
           public4 |       1731 |       1.06 | success
           public5 |       1707 |     194.42 | success
           public6 |      80172 |     287.63 | success
        ```

    * If the status is not **success**, it means your program failed in this case. E.g.,
        ```
        grading on 112062500:
          testcase |    cutsize |    runtime | status
           public1 |       fail |        TLE | public1 failed.
           public2 |       fail |        TLE | public2 failed.
           public3 |       fail |        TLE | public3 failed.
           public4 |       fail |        TLE | public4 failed.
           public5 |       fail |        TLE | public5 failed.
           public6 |       fail |        TLE | public6 failed.
        ```
