## 1 Determining the Order of Edges

Corresponding folder: `edge order`

- Instructions

### 1.1 Generating the Sorted Graph

1. Place the preprocessed datasets into the `input_graph_pre` folder.
2. Run `./preprocess_graph.sh ./input_graph_pre/ ./input_graph/` to generate files that will be placed in the `input_graph` folder.
3. Run `./run_batch.sh ./input_graph/ ./output_graph/` to generate the files with edges sorted, which will be placed in the `output_graph` folder.

### 1.2 Randomly Generate Terminal Nodes for the Corresponding Graph

- Run `./generate_terminal.sh ./input_graph/ ./output_terminal_two/ two` to generate terminal nodes with a quantity of 2. The generated terminal node files will be placed in the `output_terminal_two` folder.
- Run `./generate_terminal.sh ./input_graph/ ./output_terminal_half/ half` to generate terminal nodes with a quantity of half the total nodes. The generated terminal node files will be placed in the `output_terminal_half` folder.
- Run `./generate_terminal.sh ./input_graph/ ./output_terminal_all/ all` to generate terminal nodes with a quantity of all the nodes. The generated terminal node files will be placed in the `output_terminal_all` folder.

## 2 SDD-Based Network Reliability Compilation

Corresponding folder: `sdd based frontier search`. Place all the content from the `output_graph`, `output_terminal_two`, `output_terminal_half`, and `output_terminal_all` folders from the previous step into the `input` folder.

- Instructions

### 2.1 Constructing the Vtree

- Run `./construct_vtree arg`, where `arg` is the vtree parameter `m` as described in the paper.

> For example, when `m=4`, run `./construct_vtree 4`.

### 2.2 Reproducing the Experimental Data in the Paper

- Run `./run_res.sh arg1 arg2`, where `arg1` specifies the type of terminal nodes (`2`, `half`, `all`), and `arg2` specifies the type of data to reproduce (`1` for reproducing the size of the SDD, `2` for reproducing the time taken to compute network reliability). The results will be written into the `res.size` and `res.time` files.

> For example, running `./run_res.sh all 1` will write data into `res.size`, representing the average size of the SDD for 32 network graphs, and into `res.time`, representing the average construction time of the SDD for 32 network graphs.
>
> Running `./run_res.sh all 2` will write data into `res.size`, representing the average size of the SDD for 32 network graphs, and into `res.time`, representing the average time taken to compute network reliability for 32 network graphs.