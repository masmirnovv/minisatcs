# MiniSatCS: SAT Solving With Reified Cardinality Constraints

The source code is forked off MiniSat 2.2, with some resources (CMakeLists, test cases) from the [Production-ready MiniSAT](https://github.com/master-keying/minisat).


## Building

```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j$(nproc)
```

Run `make test` to run the test cases.

## Citation

If this project is helpful to your research, please cite our paper:
```txt
@inproceedings{jia2020efficient,
 author = {Jia, Kai and Rinard, Martin},
 booktitle = {Advances in Neural Information Processing Systems},
 editor = {H. Larochelle and M. Ranzato and R. Hadsell and M. F. Balcan and H. Lin},
 pages = {1782--1795},
 publisher = {Curran Associates, Inc.},
 title = {Efficient Exact Verification of Binarized Neural Networks},
 url = {https://proceedings.neurips.cc/paper/2020/file/1385974ed5904a438616ff7bdb3f7439-Paper.pdf},
 volume = {33},
 year = {2020}
}
```
