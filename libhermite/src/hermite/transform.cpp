#include <cmath>
#include <dlfcn.h>
#include <functional>
#include <numeric>
#include <iostream>
#include <fstream>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/function.hpp>
#include <boost/python.hpp>
#include <boost/core/ref.hpp>
#include <boost/math/special_functions/binomial.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "hermite/types.hpp"
#include "hermite/iterators.hpp"
#include "hermite/transform.hpp"

using namespace std;

namespace hermite {

void hermite_eval(double x,
        u_int degree,
        vec & values,
        bool l2)
{

    double factor = l2 ? sqrt(1 / sqrt(2*M_PI) *  exp(-x*x/2)) : 1;
    values[0] = 1*factor;
    values[1] = x*factor;

    for (unsigned i = 1; i < degree; i++)
    {
        values[i+1] = (1/sqrt(i+1))*(x*values[i]-sqrt(i)*values[i-1]);
    }
}

void map_point(vec const & translation,
        mat const & dilation,
        double *node,
        double *mapped_node)
{
    u_int dim = translation.size();

    for (u_int i = 0; i < dim; i++)
    {
        mapped_node[i] = translation[i];
        for (u_int j = 0; j < dim; j++)
        {
            mapped_node[i] += dilation[i][j]*node[j];
        }
    }
}

vec transform(
        u_int degree,
        vec const & input,
        mat const & nodes,
        mat const & weights,
        bool forward,
        bool l2)
{
    u_int dim = nodes.size();
    using boost::math::binomial_coefficient;
    u_int n_polys = (u_int) binomial_coefficient<double> (degree + dim, dim);

    u_int i,j,k;

    u_int n_points_tot = 1;
    ivec n_points(dim);
    for (i = 0; i < dim; i++)
    {
        n_points[i] = nodes[i].size();
        n_points_tot *= n_points[i];
    }

    vec output;
    if (forward)
    {
        output = vec(n_polys, 0);
    }
    else
    {
        output = vec(n_points_tot, 0);
    }

    // Compute Hermite polynomials in each dimension
    cube herm_vals_1d(dim);
    for (i = 0; i < dim; ++i)
    {
        herm_vals_1d[i] = mat(n_points[i], vec(degree + 1, 0));
        for (j = 0; j < n_points[i]; j++)
        {
            hermite_eval(nodes[i][j], degree, herm_vals_1d[i][j], l2);
        }
    }

    Hyper_cube_iterator p(n_points);
    for (i = 0; i < n_points_tot; i++, p.increment())
    {
        double weight = 1;
        if (forward)
        {
            for (j = 0; j < dim; j++)
            {
                double x = nodes[j][p[j]];
                double factor = l2 ? sqrt(2*M_PI) * exp(x*x/2) : 1;
                weight *= weights[j][p[j]]*factor;
            }
        }

        Multi_index_iterator m(dim, degree);
        for (j = 0; j < n_polys; j++, m.increment())
        {
            double val_at_point = 1;
            for (k = 0; k < dim; k++)
            {
                if (m[k] != 0)
                {
                    val_at_point *= herm_vals_1d[k][p[k]][m[k]];
                }
            }
            if(forward)
            {
                output[j] += input[i] * weight * val_at_point;
            }
            else
            {
                output[i] += input[j] * weight * val_at_point;
            }
        }
    }

    return output;
}

vec discretize(
        s_func func,
        mat const & nodes,
        vec const & translation,
        mat const & dilation)
{
    u_int dim = nodes.size();

    u_int i,j;
    double* node = (double*) malloc(sizeof(double)*dim);
    double* mapped_node = (double*) malloc(sizeof(double)*dim);

    u_int n_points_tot = 1;
    ivec n_points(dim);
    for (u_int i = 0; i < dim; i++)
    {
        n_points[i] = nodes[i].size();
        n_points_tot *= n_points[i];
    }
    vec result(n_points_tot, 0);

    Hyper_cube_iterator p(n_points);
    for (i = 0; i < n_points_tot; i++, p.increment())
    {
        for (j = 0; j < dim; j++)
        {
            node[j] = nodes[j][p[j]];
        }
        map_point(translation, dilation, node, mapped_node);
        result[i] = func(mapped_node);
    }

    free(node);
    free(mapped_node);

    return result;
}

double integrate(
        vec const & f_grid,
        mat const & nodes,
        mat const & weights)
{
    vec integral = transform(0, f_grid, nodes, weights, true, false);
    return integral[0];
}


// ---- PYTHON WRAPPERS ----
void intern_function(string const & function_body)
{
    string name = to_string(hash<string>()(function_body));
    string cpp_file = "/tmp/" + name + ".cpp";
    string so_file = "/tmp/" + name + ".so";
    ifstream test_exists(so_file.c_str());

    if(! test_exists.good()) {
         ofstream write_function;
         write_function.open(cpp_file);
         write_function << "#include <vector>\n#include <cmath>\n";
         write_function << "extern \"C\" double toIntegrate(double *v) {\n";
         write_function << "    return " << function_body << ";\n}";
         write_function.close();

        // Compile file
        string command = "c++ " + cpp_file + " -o " + so_file + " -O3 -Ofast -shared -fPIC";
        system(command.c_str());
    }
}

vec discretize_from_string(
        string function_body,
        mat const & nodes,
        vec const & translation,
        mat const & dilation)
{
    intern_function(function_body);
    string name = to_string(hash<string>()(function_body));
    string so_file = "/tmp/" + name + ".so";
    void *function_so = dlopen(so_file.c_str(), RTLD_NOW);
    s_func func = (s_func) dlsym(function_so, "toIntegrate");
    vec result = discretize(func, nodes, translation, dilation);
    dlclose(function_so);
    return result;
}


// ---- PYTHON API ----
BOOST_PYTHON_MODULE(hermite)
{
    using namespace boost::python;

    class_<vec>("double_vec")
        .def(vector_indexing_suite<vec>())
        ;

    class_<mat>("double_mat")
        .def(vector_indexing_suite<mat>())
        ;

    class_<cube>("double_cube")
        .def(vector_indexing_suite<cube>())
        ;

    def("discretize", discretize_from_string);
    def("integrate", integrate);
    def("transform", transform);
}

}
