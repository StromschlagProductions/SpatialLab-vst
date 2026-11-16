# Phase 2.2 – Praxistest-Korrekturen und akustische Diagnose

## Ergebnis und Grenzen

Die vier freigegebenen Röhren und die CRT-Signalspur bleiben erhalten. Alle übrigen Lampen, LEDs und Leuchtfelder werden wieder ausschließlich aus den Originalfrontplatten dargestellt. Ihre Überlagerungen, Pegel-/Pulsberechnungen und speziellen Repaint-Bereiche sind entfernt. Die Original-PNGs bleiben bytegleich.

Die Reglerdarstellung rotierte bisher einen kompletten 114×114-Pixel-Ausschnitt einschließlich äußerem Metallrand und reflektierten Skalenstrichen. Nun bleibt dieser äußere Rand statisch; nur die innere gebürstete Knopffläche (69 % des Ausschnittdurchmessers, mit aus dem Original bestimmten unterschiedlichen Mittelpunkten) samt Originalzeiger rotiert. Es werden die zum Skin gehörigen Originalpixel verwendet, keine neu generierte Metalltextur. Perspektive, äußerer Schatten und Reflexionen bleiben am Gerät fixiert. Windows-Aufnahmen prüfen beide Regler bei 0/25/50/75/100 % in beiden Skins.

## Audio: zunächst keine spekulative Änderung

Die Identität und die gesamte Audio-Engine entsprechen Phase 2.1. SHA-256-Prüfungen schützen Engine und Prozessorquellen. Der Vorher-/Nachher-Vergleich ergibt deshalb identischen DSP und identische Impulsantworten bei identischer Anregung; es gibt keine behauptete neue Klangabstimmung. Der bestehende ältere Phase-1/Phase-2-Vergleich bleibt als historische Referenz aktiv.

Der Auftraggeber konnte den beobachteten Bassverlust keinem konkreten Raum oder Routing mehr sicher zuordnen. Ein breitbandig fast vollständiger Bassverlust ist mit diesen Messungen nicht belegt. Daher bleiben die akustischen Änderungen gemäß der ausdrücklichen Vorgabe vorerst ausgesetzt. Die folgenden nachweisbaren Zusammenhänge begründen eine Diagnose, aber noch keine eindeutige klangliche Zielkorrektur.

### Größe und neutraler Wet-Pegel

Größe ist keine reine Lautstärkeregelung: Die nominelle Abklingzeit wird mit `2^(2*(size-0.5))` multipliziert (0,5 / 1 / 2 bei Minimum/Mitte/Maximum). Die Delaywege bleiben konstant. Die interne Early-Gewichtung fällt gleichzeitig von 1,2 auf 0,8, die Late-Gewichtung steigt von 0,65 auf 1,35. Mitte ergibt für beide den Faktor 1. Klang verändert den Tiefpass der Rückkopplung um Faktor 0,5 bis 2, nicht den breitbandigen Early-Anteil. Ein Aufdrehen beider Regler macht daher vor allem die länger stehende/hellere Nachhallkomponente hörbarer; dies beweist keinen Fehler in der Mittelstellung.

Die nominellen RT60-Werte bleiben 0,40 / 0,32 / 0,38 s. Die gemessenen kurzen breitbandigen T20-Abklingwerte aus Phase 2 sind kein Pegelmaß. Bei neutralen Reglern liegt die Energie der Late-Impulsantwort ungefähr 8–9 dB unter den Early Reflections. Bei Send −6 dB kommt eine weitere Absenkung um 6 dB gegenüber der Quellspur hinzu. Die kompakte, frühe Raumantwort kann dadurch mit der trockenen Sprache verschmelzen. Ob eine längere, dichtere oder lautere Late-Komponente gewünscht ist, lässt sich nicht allein aus einer Frequenzgangmessung bestimmen. Keine pauschale Pegelanhebung und keine Verlängerung auf Verdacht.

### Tieftonbefund

Es existiert kein Hochpass/Low-Cut im Signalweg. Die frequenzabhängige Rückkopplungsdämpfung ist ein Tiefpass. Die frühen Reflexionen bestehen aus verzögerten, teils positiv und teils negativ gewichteten Tap-Signalen; deren Summe besitzt Kammfilter und frequenzabhängige Auslöschungen. Die Kreuzmischung zwischen links/rechts sowie eine spätere Monosumme verändern diese abhängig vom Eingangssignal. Es gibt kein versehentliches Löschen des rechten Eingangskanals im geprüften Stereo-Routing; vorhandene Prozessor-Tests prüfen Mono/Stereo weiterhin.

Beispiel bei 48 kHz, neutralen Reglern, identischem Signal links/rechts, Wet-Stereo-Energie relativ zum Plugin-Eingang:

| Band | Wohnzimmer | Küche | Badezimmer |
| --- | ---: | ---: | ---: |
| 30–60 Hz | −4,01 dB | −8,54 dB | −8,11 dB |
| 60–120 Hz | −1,46 dB | −9,24 dB | −6,38 dB |
| 120–250 Hz | −3,81 dB | −1,52 dB | −3,41 dB |

Das ist ein ungleichmäßiger Wet-Frequenzgang, kein steilflankiger Bass-Cut. Bei einem 100-%-Wet-Insert fehlt der direkte Originalbass vollständig; ausschließlich die zeitlich und spektral veränderte Raumantwort bleibt. Im Send-Betrieb bleibt die trockene Spur erhalten, hinzu kommt die Raumantwort. Daraus können sich wiederum signalabhängige Interferenzen ergeben; die Messung behauptet keine transparente Insert-Wiedergabe.

`Tests/RoomDiagnostics.cpp` liefert reproduzierbare Bandmessungen bei 44,1/48 kHz für alle Räume, Größe 0/0,5/1, neutralen Klang und drei Eingangsfälle: `input=0` links=rechts, `1` nur links, `2` links=−rechts. Je Band werden 24 gleichmäßig verteilte komplexe Übertragungswerte aus einer zwei Sekunden langen Impulsantwort gemittelt. Early, Late, Stereo-Wet und Mono-Wet sind separat angegeben. Bezugspegel ist ein Eingangskanal mit Amplitude 1; bei links-only ist die Stereoausgangsenergie nicht auf die halbe Eingangsgesamtenergie normiert. CSV ist eine Diagnose und kein Hörfreigabetest. Breite Bänder und diskrete Stützstellen ersetzen keine Analyse der konkreten Intro-Musik. [Lokale Messwerte](room-diagnostics.csv); der Windows-Build erzeugt dieselbe Messung separat im Artefakt.

### Gezielter nächster Hörvergleich

Die bekannte Intro-Passage jeweils mit genau einem Raum, neutralen Reglern und dokumentiertem Stereo-/Mono-Routing prüfen: zuerst trockene Spur, dann Wet solo als Insert, dann regulär trockene Spur plus Send −6 dB / FX 0 dB. Für die Sprache ebenfalls Raum für Raum prüfen und festhalten, ob vor allem frühe Räumlichkeit oder die Hallfahne fehlt. Falls der Bassverlust wieder auftritt, Raum, Kanalmodus und Zeitstelle notieren. Erst damit lässt sich ein störender Auslöschungseffekt von erwarteter Wet-Färbung und einer gewünschten neuen Grundabstimmung unterscheiden.

## GUI-Lebenszyklus und CPU

Ein geschlossener/zerstörter Editor besitzt keinen Animationstimer. Ein direkt ausgeblendeter Editor stoppt den Timer und die Pegeltelemetrie. Wenn nur ein Host-Vorfahre verborgen wird, erkennt der Timer dies: Dann bleiben höchstens vier günstige Sichtbarkeitsabfragen pro Sekunde, ohne Lichtberechnung, Pegelabfrage oder Repaint. Beim Wiederzeigen läuft der bestehende Röhren-/CRT-Timer weiter. Zeit basiert weiterhin auf der bestehenden monotonen Uhr, kein Zurücksetzen der freigegebenen Glühfunktion.

Der bestehende **Prozessor**-Timer zur sicheren Hostbenachrichtigung beim Raumwechsel bleibt auch ohne Editor erhalten. Er kontrolliert nur eine atomare Revisionsnummer und ist keine GUI-Animation. Ihn ersatzlos abzuschalten würde die bestehende Parameterautomation gefährden. Der Audiothread veröffentlicht ohne sichtbaren Editor keine Pegel-/Blocktelemetrie.

Der GUI-Test verarbeitet eine Sekunde Audio mit 32 Instanzen ohne Editor und fordert null GUI-Telemetrie. Die Laufzeit wird informativ protokolliert; ein Rechnerwert ist keine allgemeine CPU-Garantie. Sichtbar/unsichtbar und Wiederaufnahme werden separat geprüft. Die Tests ersetzen keine Profilierung vieler Instanzen im tatsächlichen Studio-Pro-Projekt.

Der erste Windows-Lauf maß für 32 Instanzen ohne Editor rund 0,080 Sekunden für eine Sekunde Audio je Instanz (48 kHz, 128er-Blöcke); alle GUI-Zähler blieben null. Das ist ein Offline-Durchsatzwert ohne DAW und kein Idle-CPU-Profil des Hostprozesses. Der neue Sichtbarkeitstest verwendet ausdrücklich ein echtes Desktop-Peer, da JUCE einen Editor ohne sichtbares Fenster korrekterweise nicht als sichtbar behandelt.
