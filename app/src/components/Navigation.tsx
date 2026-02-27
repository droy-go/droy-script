import { useEffect, useState } from 'react';
import { Button } from '@/components/ui/button';
import { Github, Menu, X } from 'lucide-react';

const navLinks = [
  { name: 'Features', href: '#features' },
  { name: 'Playground', href: '#playground' },
  { name: 'IDE', href: '#ide' },
  { name: 'Examples', href: '#examples' },
  { name: 'Syntax', href: '#syntax' },
  { name: 'Install', href: '#installation' },
];

export default function Navigation() {
  const [isScrolled, setIsScrolled] = useState(false);
  const [isMobileMenuOpen, setIsMobileMenuOpen] = useState(false);

  useEffect(() => {
    const handleScroll = () => {
      setIsScrolled(window.scrollY > 100);
    };

    window.addEventListener('scroll', handleScroll, { passive: true });
    return () => window.removeEventListener('scroll', handleScroll);
  }, []);

  const scrollToSection = (href: string) => {
    const element = document.querySelector(href);
    if (element) {
      element.scrollIntoView({ behavior: 'smooth' });
    }
    setIsMobileMenuOpen(false);
  };

  return (
    <>
      <nav
        className={`fixed top-0 left-0 right-0 z-50 transition-all duration-500 ${
          isScrolled
            ? 'bg-[#0a0a0a]/90 backdrop-blur-xl border-b border-[#222]'
            : 'bg-transparent'
        }`}
      >
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
          <div className="flex items-center justify-between h-16">
            {/* Logo */}
            <a
              href="#"
              className="flex items-center gap-2 group"
              onClick={(e) => {
                e.preventDefault();
                window.scrollTo({ top: 0, behavior: 'smooth' });
              }}
            >
              <img
                src="/logo.png"
                alt="Droy"
                className="w-8 h-8 group-hover:scale-110 transition-transform duration-300"
              />
              <span className="font-bold text-white text-lg">Droy</span>
            </a>

            {/* Desktop Navigation */}
            <div className="hidden md:flex items-center gap-8">
              {navLinks.map((link) => (
                <button
                  key={link.name}
                  onClick={() => scrollToSection(link.href)}
                  className="text-[#a0a0a0] hover:text-white transition-colors duration-300 text-sm font-medium"
                >
                  {link.name}
                </button>
              ))}
            </div>

            {/* CTA */}
            <div className="hidden md:flex items-center gap-4">
              <Button
                variant="ghost"
                size="sm"
                className="text-[#a0a0a0] hover:text-white hover:bg-white/5"
                onClick={() =>
                  window.open('https://github.com/droy-go/droy-lang', '_blank')
                }
              >
                <Github className="w-4 h-4 mr-2" />
                GitHub
              </Button>
              <Button
                size="sm"
                className="bg-[#ff6b35] hover:bg-[#ff6b35]/90 text-white"
                onClick={() => scrollToSection('#installation')}
              >
                Get Started
              </Button>
            </div>

            {/* Mobile Menu Button */}
            <button
              className="md:hidden text-white p-2"
              onClick={() => setIsMobileMenuOpen(!isMobileMenuOpen)}
            >
              {isMobileMenuOpen ? (
                <X className="w-6 h-6" />
              ) : (
                <Menu className="w-6 h-6" />
              )}
            </button>
          </div>
        </div>
      </nav>

      {/* Mobile Menu */}
      <div
        className={`fixed inset-0 z-40 md:hidden transition-all duration-300 ${
          isMobileMenuOpen
            ? 'opacity-100 pointer-events-auto'
            : 'opacity-0 pointer-events-none'
        }`}
      >
        <div
          className="absolute inset-0 bg-black/80 backdrop-blur-xl"
          onClick={() => setIsMobileMenuOpen(false)}
        />
        <div
          className={`absolute top-16 left-0 right-0 bg-[#111] border-b border-[#222] p-4 transition-transform duration-300 ${
            isMobileMenuOpen ? 'translate-y-0' : '-translate-y-full'
          }`}
        >
          <div className="flex flex-col gap-2">
            {navLinks.map((link) => (
              <button
                key={link.name}
                onClick={() => scrollToSection(link.href)}
                className="text-left text-[#a0a0a0] hover:text-white transition-colors duration-300 py-3 px-4 rounded-lg hover:bg-[#1a1a1a]"
              >
                {link.name}
              </button>
            ))}
            <div className="border-t border-[#222] mt-2 pt-4 flex flex-col gap-2">
              <Button
                variant="ghost"
                className="justify-start text-[#a0a0a0] hover:text-white"
                onClick={() =>
                  window.open('https://github.com/droy-go/droy-lang', '_blank')
                }
              >
                <Github className="w-4 h-4 mr-2" />
                GitHub
              </Button>
              <Button
                className="bg-[#ff6b35] hover:bg-[#ff6b35]/90 text-white"
                onClick={() => scrollToSection('#installation')}
              >
                Get Started
              </Button>
            </div>
          </div>
        </div>
      </div>
    </>
  );
}
