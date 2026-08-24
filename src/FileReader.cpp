#include "FileReader.h"
#include <iostream>
#include <TBranch.h>
#include <TList.h>
#include <TChain.h>

FileReader::FileReader(const std::string& filename, const std::string& treename) : treeName(treename)
{
    OpenFile(filename, treename);
}

bool FileReader::OpenFile(const std::string& filename, const std::string& treename)
{
    CloseFile();
    this->treeName = treename;

    // std::string actualTreeName = treename;
    // size_t lastSlach = treename.find_last_of('/');
    // if (lastSlach != std::string::npos)
    // {
    //     actualTreeName = treename.substr(lastSlach + 1);
    // }

    chain = std::make_unique<TChain>(treeName.c_str());

    // std::string fullPath = filename;
    // if (lastSlach != std::string::npos)
    // {
    //     fullPath += "/" + treename;
    // }
    
    if(chain->Add(filename.c_str()) == 0)
    {
        std::cerr << "Error: Cannot open or find tree " << treeName << " in file " << filename << std::endl;
        chain.reset();
        return false;
    }

    InitializeReader();
    return true;

    // rootFile = std::make_unique<TFile>(filename.c_str(), "READ");
    // if (!rootFile || rootFile->IsZombie()) 
    // {
    //     std::cerr << "Error: Cannot open file " << filename << std::endl;
    //     return false;
    // }
    // this->treeName = treename;
    // InitializeReader();

    return true;
}

void FileReader::CloseFile()
{
    branchReaders.clear();
    treeReader.reset();
    rootFile.reset();
    chain.reset();
}

void FileReader::InitializeReader()
{
    if (!chain) return;
    treeReader = std::make_unique<TTreeReader>(chain.get());
    // if (!rootFile)
    // {return;}

    // auto* tree = dynamic_cast<TTree*>(rootFile->Get(treeName.c_str()));
    // if (!tree)
    // {
    //     std::cerr << "Error: Tree " << treeName << " not found in file" << std::endl;
    //     return;
    // }

    // treeReader = std::make_unique<TTreeReader>(tree);
}

bool FileReader::GetEntry(long long entry)
{
    if (!chain) 
        return false;

    Int_t bytesRead = chain->GetEntry(entry);
    if (treeReader)
    {
        treeReader->SetEntry(entry);
    }
    return bytesRead;
    // if (!treeReader || !treeReader->GetTree())
    // {return false;}

    // auto bytesRead = treeReader->GetTree()->GetEntry(entry);
    // std::cout << treeReader->GetTree() << '\t';
    // return bytesRead > 0;
}

long long FileReader::GetEntries()
{
    return chain ? chain->GetEntries() : -1;
    // if(!treeReader || !treeReader->GetTree())
    // {return -1;}
    // return treeReader->GetTree()->GetEntries();
}

bool FileReader::Next()
{
    return treeReader && treeReader->Next();
}

void FileReader::PrintBranches()
{
    if(!chain)
    {
        std::cerr << "No tree open" << std::endl;
        return;
    }

    auto* branches = chain->GetListOfBranches();
    if (!branches)
        return;

    std::cout << "Branches in tree " << treeName << ":" << std::endl;
    for (int i = 0; i < branches->GetEntries(); i++)
    {
        auto* branch = dynamic_cast<TBranch*>(branches->At(i));
        if (branch)
        {
            std::cout << "\t" << i << ": " << branch->GetName() 
                      << " (type: " << branch->GetClassName() << ")" << std::endl;
        }
    }
    
    // if(!treeReader || !treeReader->GetTree())
    // {
    //     std::cerr << "No tree open" << std::endl;
    //     return;
    // }
    // auto* branches = treeReader->GetTree()->GetListOfBranches();
    
    // if(!branches)
    // {return;}

    // std::cout << "Branches in tree " << treeName << " :" << std::endl;
    // for(int i=0; i<branches->GetEntries(); i++)
    // {
    //     auto* branch = dynamic_cast<TBranch*>(branches->At(i));
    //     if(branch)
    //     {
    //         std::cout << "\t" << i << ": " << branch->GetName() << "type: " << branch->GetClassName() << ")" << std::endl;
    //     }
    // }
}

TObjArray* FileReader::GetBranchList()
{
    return chain ? chain->GetListOfBranches() : nullptr;
    // if(!treeReader || !treeReader->GetTree())
    // {return nullptr;}

    // return treeReader->GetTree()->GetListOfBranches();
}