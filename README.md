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
@article{jia2020efficient,
  title={Efficient Exact Verification of Binarized Neural Networks},
  author={Jia, Kai and Rinard, Martin},
  journal={arXiv preprint arXiv:2005.03597},
  year={2020}
}
```
