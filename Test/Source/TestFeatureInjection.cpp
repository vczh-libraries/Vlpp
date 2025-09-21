#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/FeatureInjection.h"

using namespace vl;
using namespace vl::unittest;
using namespace vl::feature_injection;

class IMockFeatureImpl : public virtual IFeatureImpl
{
public:
	virtual WString GetFeatures() = 0;
};

class MockFeatureImpl : public FeatureImpl<IMockFeatureImpl>
{
public:
	// Call tracking - public for easy testing access
	vint beginInjectionCallCount = 0;
	vint endInjectionCallCount = 0;
	
	// Feature identification
	WString name;
	
	MockFeatureImpl(const WString& _name) : name(_name) {}
	
	// Override lifecycle methods to track calls
	void BeginInjection(IMockFeatureImpl* previous) override
	{
		beginInjectionCallCount++;
	}
	
	void EndInjection() override
	{
		endInjectionCallCount++;
		FeatureImpl<IMockFeatureImpl>::EndInjection();
	}
	
	// Recursive chain visualization using framework's Previous() method
	WString GetFeatures() override
	{
		auto previous = Previous();
		if (previous == nullptr)
		{
			return name;
		}
		return previous->GetFeatures() + L"-" + name;
	}
};

TEST_FILE
{
	TEST_CATEGORY(L"Basic Functionality")
	{
		TEST_CASE(L"Constructor with valid default implementation")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default");
			TEST_ASSERT(injection.Get() == defaultImpl.Obj());
		});
		
		TEST_CASE(L"Constructor with null parameter throws error")
		{
			TEST_ERROR({
				FeatureInjection<IMockFeatureImpl> injection(nullptr);
			});
		});
		
		TEST_CASE(L"Get returns current implementation")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			TEST_ASSERT(injection.Get() == defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			TEST_ASSERT(injection.Get() == impl1.Obj());
		});
	});

	TEST_CATEGORY(L"Injection Chain Management")
	{
		TEST_CASE(L"Single injection creates proper chain")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1");
			TEST_ASSERT(impl1->beginInjectionCallCount == 1);
			TEST_ASSERT(impl1->endInjectionCallCount == 0);
		});
		
		TEST_CASE(L"Multiple injections build correct chain")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			auto impl3 = Ptr(new MockFeatureImpl(L"Impl3"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1");
			
			injection.Inject(impl2.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1-Impl2");
			
			injection.Inject(impl3.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1-Impl2-Impl3");
			
			TEST_ASSERT(impl1->beginInjectionCallCount == 1);
			TEST_ASSERT(impl2->beginInjectionCallCount == 1);
			TEST_ASSERT(impl3->beginInjectionCallCount == 1);
		});
		
		TEST_CASE(L"Ejection of last implementation")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			injection.Inject(impl2.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1-Impl2");
			
			injection.Eject(impl2.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1");
			TEST_ASSERT(impl2->endInjectionCallCount == 1);
			TEST_ASSERT(impl1->endInjectionCallCount == 0);
		});
		
		TEST_CASE(L"Ejection from middle of chain")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			auto impl3 = Ptr(new MockFeatureImpl(L"Impl3"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			injection.Inject(impl2.Obj());
			injection.Inject(impl3.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1-Impl2-Impl3");
			
			// Eject impl2 should also eject impl3 (everything after impl2)
			injection.Eject(impl2.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1");
			TEST_ASSERT(impl3->endInjectionCallCount == 1);
			TEST_ASSERT(impl2->endInjectionCallCount == 1);
			TEST_ASSERT(impl1->endInjectionCallCount == 0);
		});
		
		TEST_CASE(L"Injection after ejection")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			auto impl3 = Ptr(new MockFeatureImpl(L"Impl3"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			injection.Inject(impl2.Obj());
			injection.Eject(impl1.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default");
			
			// Test injection after ejection - verify framework state reset properly
			injection.Inject(impl3.Obj());
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl3");
			TEST_ASSERT(impl3->beginInjectionCallCount == 1);
		});

		TEST_CASE(L"EjectAll restores default implementation")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			auto impl3 = Ptr(new MockFeatureImpl(L"Impl3"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			// Inject three implementations
			injection.Inject(impl1.Obj());
			injection.Inject(impl2.Obj());
			injection.Inject(impl3.Obj());
			
			// Verify current is impl3
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default-Impl1-Impl2-Impl3");
			TEST_ASSERT(injection.Get() == impl3.Obj());
			
			// EjectAll and verify default is restored
			injection.EjectAll();
			TEST_ASSERT(injection.Get()->GetFeatures() == L"Default");
			TEST_ASSERT(injection.Get() == defaultImpl.Obj());
			
			// Verify all injected implementations had EndInjection called
			TEST_ASSERT(impl1->endInjectionCallCount == 1);
			TEST_ASSERT(impl2->endInjectionCallCount == 1);
			TEST_ASSERT(impl3->endInjectionCallCount == 1);
		});
	});

	TEST_CATEGORY(L"Lifecycle Method Verification")
	{
		TEST_CASE(L"BeginInjection call count verification")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			TEST_ASSERT(impl1->beginInjectionCallCount == 0);
			TEST_ASSERT(impl2->beginInjectionCallCount == 0);
			
			injection.Inject(impl1.Obj());
			TEST_ASSERT(impl1->beginInjectionCallCount == 1);
			TEST_ASSERT(impl2->beginInjectionCallCount == 0);
			
			injection.Inject(impl2.Obj());
			TEST_ASSERT(impl1->beginInjectionCallCount == 1);
			TEST_ASSERT(impl2->beginInjectionCallCount == 1);
		});
		
		TEST_CASE(L"EndInjection call count verification")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			injection.Inject(impl2.Obj());
			
			TEST_ASSERT(impl1->endInjectionCallCount == 0);
			TEST_ASSERT(impl2->endInjectionCallCount == 0);
			
			injection.Eject(impl2.Obj());
			TEST_ASSERT(impl1->endInjectionCallCount == 0);
			TEST_ASSERT(impl2->endInjectionCallCount == 1);
			
			injection.Eject(impl1.Obj());
			TEST_ASSERT(impl1->endInjectionCallCount == 1);
			TEST_ASSERT(impl2->endInjectionCallCount == 1);
		});
		
		TEST_CASE(L"Ejection calls EndInjection in correct order")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			auto impl3 = Ptr(new MockFeatureImpl(L"Impl3"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			injection.Inject(impl2.Obj());
			injection.Inject(impl3.Obj());
			
			// Eject impl1 should call EndInjection on impl3, impl2, then impl1
			injection.Eject(impl1.Obj());
			TEST_ASSERT(impl1->endInjectionCallCount == 1);
			TEST_ASSERT(impl2->endInjectionCallCount == 1);
			TEST_ASSERT(impl3->endInjectionCallCount == 1);
		});
	});

	TEST_CATEGORY(L"Error Handling")
	{
		TEST_CASE(L"Inject null implementation throws error")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			TEST_ERROR({
				injection.Inject(nullptr);
			});
		});
		
		TEST_CASE(L"Eject null implementation throws error")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			TEST_ERROR({
				injection.Eject(nullptr);
			});
		});
		
		TEST_CASE(L"Eject non-existent implementation throws error")
		{
			auto defaultImpl = Ptr(new MockFeatureImpl(L"Default"));
			auto impl1 = Ptr(new MockFeatureImpl(L"Impl1"));
			auto impl2 = Ptr(new MockFeatureImpl(L"Impl2"));
			auto nonExistent = Ptr(new MockFeatureImpl(L"NonExistent"));
			FeatureInjection<IMockFeatureImpl> injection(defaultImpl.Obj());
			
			injection.Inject(impl1.Obj());
			injection.Inject(impl2.Obj());
			
			TEST_ERROR({
				injection.Eject(nonExistent.Obj());
			});
		});
	});
}