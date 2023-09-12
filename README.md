# pvtrace
Visualize function calls with Graphviz (Source code by M. Tim Jones)

By utilizing the gcc compiler option -finstrument-functions a dynamic function address trace can be recorded. The pvtrace utility provide by this project converts that record into a dot graph that can then be visualised.

An article containing full details of this procedure and how it works behind the scenes was written by the pvtrace source code author M. Tim Jones.  This article was available on IBMs developer works website in 2005 and a pdf version can be found in this repository.

After downloading the source code use make to create the pvtrace executable.

The process for creating the call trace graph of your application is as follows:

# Compile in the instrumentation
gcc -g -finstrument-functions instrument.c all-your-apps-source-files.c -o exename

# Run the executable to generate the stack trace (output is trace.txt)
./exename

# Convert the trace.txt to a graphviz dot file (output is graph.dot)
pvtrace exename

# Convert the dot graph into a jpeg for visualisation (output is graph.jpg)
dot -Tjpg graph.dot -o graph.jpg

--------------------------------------------------------------------------------
![image](https://github.com/zxgtz/pvtrace/assets/4532703/0be0af1f-d3df-445b-8d8a-b393a813c01d)
20230912:
  - 将函数数目及函数名字符数限制增大，以能处理较大的项目；
  - 正确生成C++函数名；
  - 增加进出函数的时刻；
  - 记录函数执行时间；
