# Distributed N-Body Simulation

Hello there! This program performs gravitational n-body simulation,
designed to run on small computing clusters.

## Requirements
- **MPI**: MPICH recommended. (Other MPI implementations should work, but have not been tested.)

Rendering requirements (optional)

- **GPU**: Support for OpenGL 3.3+
- **GLAD**: 
- **GLFW**: 


## Installation

### Install Dependencies:
Install an MPI implementation. MPICH can be installed using:
```bash
sudo apt install mpich
```
Make sure that `mpic++` is available from path.

### Install Rendering Dependencies (optional)

```bash
sudo apt install libglfw3-dev
```
install [GLAD](https://glad.dav1d.de/)

### Build

**Clone Repository:**
```bash
git clone https://github.com/Grotball/Distributed-Nbody-Simulation.git
cd Distributed-Nbody-Simulation
```
**Run Makefile:**

```bash
# Without rendering features
make

# With rendering features
make enable_opengl=1 glad_dir=path/to/glad
```


## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.