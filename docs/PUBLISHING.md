# Veröffentlichung und Herstellerangaben

**Öffentlicher Hersteller:** Stromschlag Audio. **GitHub-Projekt:** StromschlagProductions/SpatialLab-vst.

Änderungen dürfen mit beliebigen Entwicklungswerkzeugen (Claude, Codex, ChatGPT, IDE) erstellt werden. Diese Werkzeuge sind keine Plugin-Hersteller. In JUCE/CMake, VST3-Metadaten, Benutzeroberfläche, README und Release-Titeln wird Stromschlag Audio beziehungsweise Stromschlag Productions als Marke verwendet. Historische Git-Commit-Autoren und Co-Autoren bleiben unverändert; fremde Lizenz- und Urheberrechtshinweise dürfen nicht entfernt werden.

## Release-Ablauf

1. Code prüfen und nach `main` übernehmen.
2. Build, Regressionstests und pluginval Strictness 8 erfolgreich abschließen.
3. Bestehenden, zur Plugin-Version passenden Release-Tag anlegen und den Workflow **Publish verified SPATIAL LAB release** mit genau passendem erfolgreichen vollständigen **Build and test**-Abnahmelauf starten. Die zusätzliche Herkunfts-, ZIP- und Prüfsummenvalidierung bleibt verpflichtend.
4. Prüfen, ob `/releases/latest` auf ein veröffentlichtes stabiles Release mit Windows-VST3-ZIP zeigt.
5. Die Stromschlag-Homepage liest dieses Release automatisch. Bei GitHub-API-Problemen führt ihr Fallback zur aktuellen Release-Seite.

Ein Git-Push oder erfolgreicher Build ist **nicht** automatisch eine Freigabe. Keine automatische öffentliche Veröffentlichung bei jedem Commit. So werden unfertige Builds und unnötige Windows-CI-Kosten vermieden.

Übergreifender Standard: https://github.com/StromschlagProductions/StromschlagProductions.github.io/blob/main/docs/RELEASE_STANDARD.md

## Öffentliches Versionsschema ab 1.3

CMake ist die Versionsquelle. Intern kann JUCE/VST3 beispielsweise 1.3.0 verwenden; öffentlich werden Patch-Nullen weggelassen: Tag `v1.3`, Titel `SPATIAL LAB v1.3`, Paket `SpatialLab-v1.3-Windows-x64-VST3.zip`. Historische Tags werden nicht umbenannt. Die GUI verwendet den generierten Header `SpatialLabBranding.h`, keine manuell eingetragene Versionszahl.

Vor dem Taggen werden die vier echten Windows-Aufnahmen aus der erfolgreichen Abnahme mit `Tools/package_branding_screenshots.py <capture-root> . --run <run-id>` bytegleich nach `docs/` übernommen. Das Werkzeug erstellt `docs/screenshots.json` mit tatsächlicher Capture-Version und Prüfsummen. Bilder und Manifest gemeinsam committen; keine Versionsnummer im Manifest nachträglich ersetzen. Den Tag auf dem finalen `main` mit Dokumentation und Bildern anlegen. Der Veröffentlichungsworkflow vergleicht alle Build-Eingaben mit dem akzeptierten Build; reine Dokumentations-/Bildnachträge benötigen keinen zweiten Build.

Der Workflow hängt das geprüfte Installationspaket, `SHA256SUMS.txt` und `screenshots.json` an das stabile Release. Erst anschließend Homepage, dynamische Versionsanzeige, native Bilddatei und tatsächliches ZIP-Ziel prüfen. Andere Plugin-Einträge bleiben unverändert.
