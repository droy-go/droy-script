import { useEffect } from 'react';
import Navigation from '@/components/Navigation';
import Hero from '@/sections/Hero';
import Features from '@/sections/Features';
import Playground from '@/sections/Playground';
import IDESection from '@/sections/IDESection';
import CodeExamples from '@/sections/CodeExamples';
import LanguageSyntax from '@/sections/LanguageSyntax';
import Installation from '@/sections/Installation';
import Footer from '@/sections/Footer';

function App() {
  useEffect(() => {
    // Add noise overlay class to body
    document.body.classList.add('noise');
    
    return () => {
      document.body.classList.remove('noise');
    };
  }, []);

  return (
    <div className="min-h-screen bg-[#0a0a0a] text-white">
      <Navigation />
      <main>
        <Hero />
        <Features />
        <Playground />
        <IDESection />
        <CodeExamples />
        <LanguageSyntax />
        <Installation />
      </main>
      <Footer />
    </div>
  );
}

export default App;
