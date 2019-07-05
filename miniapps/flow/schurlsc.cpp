#include "schurlsc.hpp"
#include "_hypre_parcsr_ls.h"

namespace mfem
{

SchurLSC::SchurLSC(HypreParMatrix *C, HypreParMatrix *B) :
    Operator(C->Height(), B->Width()),
    A_(nullptr), B_(B), C_(C)
{
    CB_ = ParMult(C_, B_);
    amgCB_ = new HypreBoomerAMG(*CB_);
    amgCB_->SetPrintLevel(0);
    HYPRE_Solver amg_precond = static_cast<HYPRE_Solver>(*amgCB_);

    HYPRE_BoomerAMGSetCoarsenType(amg_precond, 6);
    HYPRE_BoomerAMGSetAggNumLevels(amg_precond, 0);
    HYPRE_BoomerAMGSetRelaxType(amg_precond, 6);
    // HYPRE_BoomerAMGSetNumSweeps(amg_precond, 1);
    // HYPRE_BoomerAMGSetStrongThreshold(amg_precond, 0.25);
    HYPRE_BoomerAMGSetInterpType(amg_precond, 0);
    HYPRE_BoomerAMGSetPMaxElmts(amg_precond, 0);
    // HYPRE_BoomerAMGSetMaxLevels(amg_precond, 25);

    x0.SetSize(B->Height());
    y0.SetSize(B->Height());
    x1.SetSize(C->Height());
}

void SchurLSC::Mult(const Vector &x, Vector &y) const
{
    if (A_ == nullptr)
    {
        mfem_error("SchurLSC: Set A before trying to solve.");
    }

    amgCB_->Mult(x, x1);
    B_->Mult(x1, x0);
    A_->Mult(x0, y0);
    C_->Mult(y0, x1);
    amgCB_->Mult(x1, y);
}

SchurLSC::~SchurLSC()
{
    delete CB_;
    delete amgCB_;
}

}
