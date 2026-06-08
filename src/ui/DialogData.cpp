#include "DialogData.h"

std::map<std::string, DialogNode> buildDialogData() {
    std::map<std::string, DialogNode> data;

    // Tree 1: Kamil after printer sabotage
    {
        DialogNode n;
        n.id = "kamil_printer_start";
        n.speaker = "Kamil";
        n.text = "O, Marcin! Znowu ta cholerna drukarka sie zaciela. Mysle, ze to Windows Update cos znowu nawalil.";
        n.choices.push_back({"[1] Tak, Windows Update to katastrofa.", "kamil_p2", 0.0f});
        n.choices.push_back({"[2] Mhm... dziwne.", "kamil_p3", 0.0f});
        data["kamil_printer_start"] = n;
    }
    {
        DialogNode n;
        n.id = "kamil_p2";
        n.speaker = "Kamil";
        n.text = "No wlasnie! Juz trzeci raz w tym miesiacu. Musze zglosic to do IT.";
        n.choices.push_back({"[1] Dobra, powodzenia.", "", 0.0f});
        data["kamil_p2"] = n;
    }
    {
        DialogNode n;
        n.id = "kamil_p3";
        n.speaker = "Kamil";
        n.text = "Cos podejrzanego? Nie, na pewno to Windows.";
        n.choices.push_back({"[1] Pewnie masz racje.", "", 0.0f});
        data["kamil_p3"] = n;
    }

    // Tree 2: Grazyna suspicious
    {
        DialogNode n;
        n.id = "grazyna_suspicious_start";
        n.speaker = "Grazyna";
        n.text = "Marcin, widzialam cie przy drukarce. Wszystko w porzadku?";
        n.choices.push_back({"[1] Tak, probowalem ja naprawic.", "grazyna_s2", -5.0f});
        n.choices.push_back({"[2] Nie bylem przy zadnej drukarce.", "grazyna_s3", 10.0f});
        data["grazyna_suspicious_start"] = n;
    }
    {
        DialogNode n;
        n.id = "grazyna_s2";
        n.speaker = "Grazyna";
        n.text = "Aha... rozumiem. Nastepnym razem moze lepiej wezwac serwis.";
        n.choices.push_back({"[1] Oczywiscie, masz racje.", "", -3.0f});
        data["grazyna_s2"] = n;
    }
    {
        DialogNode n;
        n.id = "grazyna_s3";
        n.speaker = "Grazyna";
        n.text = "Naprawde? Bo widzialam cie z teczka...";
        n.choices.push_back({"[1] Mylylas mnie z kims innym.", "", 5.0f});
        data["grazyna_s3"] = n;
    }

    // Tree 3: Tomasz manager
    {
        DialogNode n;
        n.id = "tomasz_manager_start";
        n.speaker = "Tomasz";
        n.text = "Marcin, musze porozmawiac o wczorajszym incydencie z drukarką. Kto za to odpowiada?";
        n.choices.push_back({"[1] Nie wiem, Tomku, ale Kamil krecil sie tam rano.", "tomasz_m2", -10.0f});
        n.choices.push_back({"[2] Zajmuje sie tym.", "tomasz_m3", 0.0f});
        data["tomasz_manager_start"] = n;
    }
    {
        DialogNode n;
        n.id = "tomasz_m2";
        n.speaker = "Tomasz";
        n.text = "Kamil? To ma sens. Przekaze to dalej. Dzieki za info.";
        n.choices.push_back({"[1] Zawsze do uslug.", "", 0.0f});
        data["tomasz_m2"] = n;
    }
    {
        DialogNode n;
        n.id = "tomasz_m3";
        n.speaker = "Tomasz";
        n.text = "Bardzo profesjonalnie. Licze na raport do piatku.";
        n.choices.push_back({"[1] Oczywiscie.", "", 0.0f});
        data["tomasz_m3"] = n;
    }

    return data;
}
