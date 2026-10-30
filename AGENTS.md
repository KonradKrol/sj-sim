# Sj.Sim UI principles

For all UI work, prioritize these requirements from the project owner:

1. **Hierarchy**: make the main information, primary action, secondary controls,
   and supporting details clearly distinct through layout, typography and spacing.
2. **Contrast**: distinguish selected states, actions, sections and data without
   excessive decoration. Preserve readability in light and dark themes.
3. **Clarity**: purpose and scope must be apparent before interaction. Use direct,
   conservative Polish labels, visible context and established interaction patterns.
4. **Low cognitive load**: remove visual noise and redundant information. Use
   spacing to group related controls and separate unrelated decisions.
5. **Understandable language**: explain actions, warnings and consequences in
   plain language, without requiring knowledge of implementation details.

Hierarchy and clarity take precedence over decoration. Maintain the expressive
alpine identity, compact desktop layout and usability on small laptop screens.
Use animations to explain state and navigation; respect reduced motion.

Work on the feature branch, never main. Work without subagents. Commit completed
changes separately and push after each commit, as requested by the owner.
