////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_FEATURES_INDEX_POOL_FEATURE_H
#define APPLICATION_FEATURES_INDEX_POOL_FEATURE_H 1

#include "ApplicationFeatures/ApplicationFeature.h"

namespace arangodb {
namespace basics {
class ThreadPool;
}

class IndexPoolFeature final : public application_features::ApplicationFeature {
 public:
  explicit IndexPoolFeature(application_features::ApplicationServer* server);

 public:
  void collectOptions(std::shared_ptr<options::ProgramOptions>) override final;
  void validateOptions(std::shared_ptr<options::ProgramOptions>) override final;
  void start() override final;
  void unprepare() override final;

  basics::ThreadPool* getThreadPool() const { return _indexPool.get(); }

 private:
  uint64_t _indexThreads;
  std::unique_ptr<basics::ThreadPool> _indexPool;
};
}

#endif
