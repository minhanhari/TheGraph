# TheGraph

## Исследование влияния РНК-связывающих белков (РСБ) на разные виды рака

_Посттранскрипционные модификации_ — это изменения, которые происходят с вновь транскрибированным транскриптом первичной РНК (hnRNA) после того, как произошла транскрипция, но до ее трансляции в белковый продукт, которые приводят к превращению первичного транскрипта в зрелую РНК. Контроль экспрессии генов на этом этапе, который называется _посттранскрипционной регуляцией_, в котором взаимодействия РНК-белок являются ключевыми факторами, включая сплайсинг РНК, кэпирование, ядерный экспорт, деградацию, субклеточную локализацию и трансляцию.
На регуляторные процессы влияет разнообразная популяция _РНК-связывающих белков_ (РСБ) или _микроРНК_, и установлено, что их нарушение регуляции связано с многочисленными заболеваниями и нарушениями, такими как нейродегенеративные заболевания или рак. Многие исследования показывают, что, поскольку многие RBP и микроРНК нацелены на одни и те же транскрипты мРНК, вполне вероятно, что между этими транс-регуляторами существуют конкурентные или кооперативные взаимодействия, которые создают необычное поведение при экспрессии генов. В этом исследовании мы найдем группы RBP, которые с большей вероятностью будут играть важную роль в различных типах рака, наблюдая за их ко-регулирующими связями среди других транс-факторов в определенных генах или типах клеток.

### Создание графа совместной регуляции РНК-связывающих белков

Во-первых, нужно определить, какие РСБ нацелены на гены во время посттранскрипционных событий, которые мы хотим наблюдать. Эти данные можно посмотреть в общедоступной базе данных AURA (Atlas of UTR Regulatory Activity). Он содержит информацию о трансфакторах, а также об их генах-мишенях и сайтах связывания. Мы также можем построить граф совместного регулирования РСБ для клеточных линий. Поскольку BioGPS предоставляет библиотеку профилей экспрессии мРНК для клеточных линий, мы можем искать списки генов для определенной клеточной линии, а затем создавать карту РСБ из базы данных AURA вместе с их сайтами связывания (генами).
Узнав список трансфакторов, мы создаем связи между ними. Поскольку связи просто основаны на совместной регуляции, нам необходимо выяснить, какие пары РСБ воздействуют на одни и те же гены. Мы можем ограничить связи количеством генов, которые они совместно регулируют (более 2, 3 или 4 вместо одного).

### Определение групп РСБ, имеющих отношение с типами рака

Теперь используем программную систему для создания графических моделей наиболее важных РСБ для каждого типа рака. В данной работе, исследуем 9 типов рака, которые наиболее рас-пространенные и опасные для человека. Их названия и сокращения указаны в таблице 2.
_Таблица 2 – TCGA исследовательские сокращения_
|Исслед. сокращения |Исследовательские название |
|-------------------|---------------------------------------|
|BLCA |Bladder Urothelial Carcinoma |
|BRCA |Breast invasive carcinoma |
|COAD |Colon adenocarcinoma |
|READ |Rectum adenocarcinoma |
|GBM |Glioblastoma multiforme |
|HNSC |Head and Neck squamous cell carcinoma |
|KICH |Kidney Chromophobe |
|KIRC |Kidney renal clear cell carcinoma |
|KIRP |Kidney renal papillary cell carcinoma |

Мутации играют фундаментальную роль в развитии рака, и многие из них создают уязвимые места. Давно признано, что некоторые определенные гены чаще мутируют и, следовательно, имеют отношение к здоровью и болезням человека. Геномное секвенирование ДНК из образцов опухолей больных раком в настоящее время охарактеризовало относительное количество мутаций для всех генов и для многих различных форм рака. Списки наиболее часто мутируемых генов собираются из The Cancer Genome Atlas Program (TCGA) через Genomic Data Commons (GDC) Национального института рака США. Атлас генома рака, знаковая программа геномики рака, молекулярно охарактеризовал более 20 000 первичных раковых заболеваний и сопоставил нормальные образцы, охватывающие 33 типа рака.
Получив списки необходимых генов, мы сможем создать графовую модель для каждого из них, используя подсистему обработки данных, в чем показывается отношения совместной регуляции РНК-связывающих белков.

_Центральность_ — это основная метрика, определяющая значимые узлы в графе среди прочих. Вес ребер также был принят во внимание, потому что вес ребра эквивалентен количеству генов, которые совместно регулируются двумя RBP, и имеет смысл, что чем больше вес, т. e. чем больше генов, которые RBP совместно регулирует с другими, тем более он значим в графовой модели. Из списков мутировавших генов 9 видов рака вычислены, какие трансфакторы более ответственны за мутацию в каждом случае.
