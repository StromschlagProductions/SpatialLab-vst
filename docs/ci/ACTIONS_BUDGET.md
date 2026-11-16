# Actions-Verbrauch ab Phase 3B

Zuvor lösten Push und Pull Request jeweils dieselbe vollständige Windows-/Linux-Pipeline aus. Dadurch entstanden zwei Windows-Builds pro Zwischenstand. Concurrency brach überholte Läufe innerhalb derselben Ref ab, verhinderte aber keine Push-/PR-Doppelung.

Jetzt gilt:

- Kein Push-Trigger mehr. Relevante Pull-Request-Änderungen führen automatisch nur die drei kurzen Linux-Sanitizer-Tests DSP, RetroMotion und RoomVoicing aus.
- Pfadfilter berücksichtigen Source, Tests, Tools, Assets, CMakeLists und die Workflowdatei. Reine README-/Dokumentationsänderungen starten keine CI.
- Windows-VST3, sämtliche Prozessor-/GUI-Tests, Beleuchtungsprüfungen, Screenshots, Messungen und Installationspaket bleiben unverändert vollständig erhalten. Sie laufen gezielt per workflow_dispatch mit full_windows=true.
- Die lange Environments-Prüfung (alle 26 IRs, Makrovarianten und 19 Referenzantworten) bleibt vollständig erhalten und läuft zusammen mit den anderen drei Sanitizer-Tests bei jeder manuellen Abnahme. Sie dauerte zuletzt rund vier Minuten und wird nicht für jeden Zwischencommit wiederholt.
- Ein manueller Lauf mit full_windows=false führt nur Linux aus, einschließlich der vollständigen vier Tests.
- Die bestehende concurrency-Gruppe mit cancel-in-progress bleibt aktiv; ein neuer Lauf derselben Ref ersetzt den überholten Lauf. Zusätzliche automatische Release-/Merge-/Deployment-Aktionen gibt es nicht.
- Während der Entwicklung lokale DSP-/Syntax-/Bildprüfungen verwenden. Vor einer Abnahme den vollständigen Lauf am endgültigen Code-Stand starten und dessen Commit/Artefakte im PR verlinken. Ein grüner kurzer Linux-PR-Check allein ersetzt die vollständige Windows-/Linux-Abnahme nicht.

Manuell für den GUI-Branch starten:

```sh
gh workflow run build.yml --repo StromschlagProductions/SpatialLab-vst --ref develop/retro-gui -f full_windows=true
```

Linux-only entsprechend mit `-f full_windows=false`. Ein erneuter vollständiger Lauf ist bei neuen Plugin-/GUI-Codeänderungen oder relevanten Fehlern erforderlich, nicht für reine Nachweis-/Dokumentationsänderungen.

Der erfolgreiche Phase-3B-Lauf 43941486329 bleibt der Buildnachweis. Nachfolgende Kontingent-Änderungen wurden lokal auf gültige YAML-Struktur und unveränderte Windows-Prüfschritte und die explizite vollständige Linux-Testausführung geprüft; dafür wird kein weiterer Windows-Build gestartet. Der abschließende Nachweiscommit verwendet [skip ci], um auch keinen unnötigen Linux-Lauf für die Dokumentation/Messauswertung zu starten.

Keine exakte Restminutenzahl behauptet: Der Auftraggeber meldete etwa 188 verfügbare Minuten; Abrechnung, Runner-Gewichtung und Aktualisierungsverzögerungen sind im Konto nachzusehen.


## Vollabnahme ohne Workflow auf main

Die Default-Branch-Prüfung ergab: `.github/workflows/build.yml` fehlt derzeit auf main. GitHub kann workflow_dispatch deshalb bis zur späteren Übernahme auf die Default-Branch nicht zuverlässig anbieten. Es wird dafür nichts auf main geschrieben oder gemergt.

Für den bestehenden Draft-PR #2 ist alternativ das bewusste Hinzufügen des Labels `ci:full` vorgesehen:

```sh
gh pr edit 2 --repo StromschlagProductions/SpatialLab-vst --remove-label ci:full
gh pr edit 2 --repo StromschlagProductions/SpatialLab-vst --add-label ci:full
```

Falls das Label noch fehlt, einmalig im Repository anlegen (`gh label create ci:full --repo StromschlagProductions/SpatialLab-vst --description 'Explicit complete Windows/Linux acceptance' --color 8A2BE2`). Danach nur dann zum PR hinzufügen, wenn eine vollständige Abnahme gewünscht ist. Ein liegengebliebenes Label startet **keinen** weiteren Windows-Lauf bei synchronize: Nur das neue labeled-Ereignis zählt. Andere Labels starten keinen Runner. Die vollständige Linux-Suite läuft beim Vollabnahme-Label mit; normale relevante PR-Updates führen nur die drei kurzen Tests aus. Concurrency derselben PR bricht überholte Abnahmen ab.

Das Repository-Label wurde angelegt, aber keinem PR hinzugefügt. Der Labelweg wurde konfiguriert, aber nicht ausgelöst: Der bereits erfolgreiche Phase-3B-Lauf wird nicht wiederholt. Nach späterer Übernahme der Workflowdatei auf die Default-Branch steht zusätzlich der oben dokumentierte workflow_dispatch-Weg bereit.
