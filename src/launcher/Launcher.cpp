//
// Created by ivan on 08.08.16.
//
#include <iostream>

#include "Launcher.h"
#include "src/libsolvers/LaplaceSolver/PrimalSolver.hpp"
#include "src/libsolvers/Postprocessor/OutputResults.hpp"

Launcher::Launcher ()
{

}

void Launcher::set_task (const std::string &taskName)
{
    this->taskName = taskName;
    boost::system::error_code err;
    if (!bfs::exists (this->taskName, err))
    {
        throw bfs::filesystem_error (taskName, err);
    }
}

void Launcher::set_output_dir (const std::string &outputDirName)
{
    outputDir = outputDirName;

    if (!bfs::exists (outputDir))
    {
        std::cout << "Output direction does not exist. Trying to create it."
                  << std::endl;
        bfs::create_directories (outputDir);
    }
}

void Launcher::run ()
{
    std::cout << "Running task: " << taskName.filename ()
              << " from: " << bfs::canonical (taskName).remove_filename ()
              << std::endl;

    std::cout << "Output will be writed in: " << bfs::canonical (outputDir)
              << std::endl;

    dealii::Triangulation<2> triangulation;
    dealii::GridGenerator::hyper_cube (triangulation, 0.0, 1.0);

    dealii::FE_Q<2> fe (2);
    dealii::QGauss<2> quadrature (2);
    dealii::ZeroFunction<2> zero (1);
    dealii::ConstantFunction<2> one (1.0, 2);

    LaplaceSolver::PrimalSolver<2> solver (triangulation,
                                           fe,
                                           quadrature,
                                           zero,
                                           one);
    solver.refine_grid ();
    solver.refine_grid ();
    solver.solve_problem ();

    Postprocessor::OutputResults<2>
        outputResults (outputDir.string (),
                       "solution",
                       dealii::DataOutBase::vtu);

    solver.postprocess (outputResults);

    std::cout << "Task completed." << std::endl;
}
